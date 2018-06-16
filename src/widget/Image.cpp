//
// Created by Raphael on 15.06.2018.
//

#include "Image.h"
#include "../helper/CompilerMacros.h"
#include "../manager/ShaderManager.h"

Image::Image(float x, float y, float width, float height, const std::shared_ptr<Texture> &image)
        :  imgTexture(image), shader(ShaderManager::load("hud")), x(x), y(y) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    this->resize(width, height);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

    glBindVertexArray(0);
}

void Image::render(const glm::mat4 &projection, float screenGamma) {
    shader->use();
    imgTexture->bind(GL_TEXTURE0);

    shader->setUniform("Texture", 0);
    shader->setUniform("projection", projection);
    shader->setUniformIfNeeded("screenGamma", screenGamma);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
}

void Image::resize(float width, float height) {
    this->glBuffer = {
            x        , y          , 0.0f, 0.0f,
            x + width, y          , 1.0f, 0.0f,
            x        , y + height , 0.0f, 1.0f,
            x + width, y          , 1.0f, 0.0f,
            x + width, y + height , 1.0f, 1.0f,
            x        , y + height , 0.0f, 1.0f,
    };

    glBufferData(GL_ARRAY_BUFFER, glBuffer.size() * sizeof(float), glBuffer.data(), GL_STATIC_DRAW);
}

Image::~Image() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Image::setImage(const std::shared_ptr<Texture> &image) {
    this->imgTexture = image;
}


