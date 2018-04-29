//
// Created by Raphael on 01.04.2018.
//

#include "Label.h"
#include "../manager/ShaderManager.h"

#include <utility>

Label::Label(std::string text, std::shared_ptr<Font> font, float x, float y, float scale, glm::vec3 color)
        : color(color), x(x), y(y), scale(scale), font(std::move(font)), text(text) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    this->shader = ShaderManager::load("font");
    buildVBO();
}

Label::~Label() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Label::render(const glm::mat4 &projection) {
    shader->use();
    shader->setUniform("textColor", this->color);
    shader->setUniform("projection", projection);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // Bind Texture atlas
    glBindTexture(GL_TEXTURE_2D, font->getAtlasTexID());

    // Update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Render quads
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vboData.size() * 6));

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_BLEND);
}

float Label::getWidth() {
    float width = 0.0f;

    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Font::Character ch = font->get((unsigned long) *c);
        width += (ch.advance >> 6) * scale;
    }

    return width;
}

void Label::setPosition(float x, float y) {
    this->x = x;
    this->y = y;
}

void Label::resize(float x, float y) {}

void Label::setText(std::string text) {
    this->text = text;
    buildVBO();
}

void Label::buildVBO() {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    float x = this->x;
    float y = this->y;

    vboData.clear();
    for (auto &c : this->text) {
        Font::Character ch = font->get((unsigned long) c);

        const GLfloat xpos = x + ch.bearing.x * scale;
        const GLfloat ypos = y + (ch.size.y - ch.bearing.y) * scale;

        const GLfloat w = ch.size.x * scale;
        const GLfloat h = ch.size.y * scale;

        const Quad q {
            .vertices = {
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

    glBufferData(GL_ARRAY_BUFFER, vboData.size() * sizeof(Quad), vboData.empty() ? nullptr : &vboData.at(0), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
