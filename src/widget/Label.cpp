//
// Created by Raphael on 01.04.2018.
//
#include <utility>

#include "Label.h"

#include "../manager/ShaderManager.h"
#include "../helper/CompilerMacros.h"

Label::Label(std::string text, std::shared_ptr<Font> font, float x, float y, float scale, glm::vec3 color)
        : color(color), x(x), y(y), scale(scale), font(std::move(font)), text(text) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Create a valid .data() pointer
    this->vboData.emplace_back();
    this->vboData.clear();

    glBindVertexArray(VAO);
    buildVBO();

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    this->shader = ShaderManager::load("font");
}

Label::~Label() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Label::render(const glm::mat4 &projection, float screenGamma) {
    shader->use();
    shader->setUniform("textColor", this->color);
    shader->setUniform("projection", projection);
    shader->setUniformIfNeeded("screenGamma", screenGamma);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    glBindVertexArray(VAO);

    // Bind Texture atlas
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font->getAtlasTexID());

    // Render quads
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vboData.size() * 6));

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
}

float Label::getWidth() {
    return w;
}

void Label::setPosition(float x, float y) {
    if (this->x != x || this->y != y) {
        this->x = x;
        this->y = y;

        buildVBO();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Label::resize(float UNUSED(x), float UNUSED(y)) {}

void Label::setText(const std::string &text) {
    if (this->text != text) {
        this->text = text;
        buildVBO();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Label::buildVBO() {
    float x = this->x;
    float y = this->y;
    this->w = 0;

    vboData.clear();
    vboData.reserve(this->text.size());

    for (auto &c : this->text) {
        Font::Character ch = font->get((unsigned long) c);

        const GLfloat xpos = x + ch.bearing.x * scale;
        const GLfloat ypos = y + (ch.size.y - ch.bearing.y) * scale;

        const GLfloat w = ch.size.x * scale;
        const GLfloat h = ch.size.y * scale;

        const Quad q {
            {
                { xpos,     ypos - h,   ch.tx                 , 0.0           },
                { xpos,     ypos,       ch.tx                 , ch.texCoord.y },
                { xpos + w, ypos,       ch.tx + ch.texCoord.x , ch.texCoord.y },

                { xpos,     ypos - h,   ch.tx                 , 0.0           },
                { xpos + w, ypos,       ch.tx + ch.texCoord.x , ch.texCoord.y },
                { xpos + w, ypos - h,   ch.tx + ch.texCoord.x , 0.0           }
            }
        };
        vboData.push_back(q);

        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
    }

    w = x - this->x;

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vboData.size() * sizeof(Quad), vboData.data(), GL_DYNAMIC_DRAW);
}
