//
// Created by raphael on 13.03.18.
//

#include <glad/glad.h>
#include "Triangle.h"
#include "../manager/ShaderManager.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float Triangle::vertices[] = {
        // positions:           // texture cords:
        -0.5f, -0.5f, 0.0f,      0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,      1.0f, 0.0f,
         0.0f,  0.5f, 0.0f,      0.5f, 1.0f
};

Triangle::Triangle(const glm::vec3 &pos, const std::shared_ptr<Texture> &texture): Object3D(ShaderManager::load("triangle")) {
    this->texture = texture;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position:
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);

    // Texture Cords:
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    this->texture->bind(GL_TEXTURE0);
}

void Triangle::draw() {
    // Bind texture to texture unit 0
    this->shader->setUniform("ourTexture", 0);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

Triangle::~Triangle() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
