//
// Created by raphael on 13.03.18.
//

#include <utility>

#include "Object3D.h"

#include "../Scene.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>

void Object3D::render(Scene *scene) {
    for (auto &shader : shaders) {
        shader->use();
        shader->setUniformIfNeeded("model", this->model);
        shader->setUniform("view", scene->getCamera().getViewMatrix());
        shader->setUniform("projection", scene->getCamera().getProjectionMatrix());

        shader->setUniformIfNeeded("camera_position", scene->getCamera().getPosition());
        shader->setUniformIfNeeded("screenGamma", scene->gamma);

        const auto lightsPos = shader->getLocation("lights");
        if (lightsPos >= 0) {
            shader->setUniform("lights", (int) scene->getLights().size());
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, scene->getLightVBO());
        }

        this->draw(shader);
        glUseProgram(0);
    }
}

Object3D::Object3D(const glm::vec3 &position, float boundingSphereRadius, const std::vector<std::shared_ptr<Shader>> &shader) {
    this->shaders = shader;
    this->boundingSphereRadius = boundingSphereRadius;

    setOrigin(position);
}

void Object3D::rotate(float angle, const glm::vec3 &vec) {
    this->model = glm::rotate(this->model, glm::radians(angle), vec);
}

void Object3D::translate(const glm::vec3 &vec) {
    this->position = vec;
    this->model = glm::translate(this->model, vec);
}

void Object3D::rotate(const glm::quat &quat) {
    this->model = this->model * glm::toMat4(quat);
}

void Object3D::setOrigin(const glm::vec3 &vec) {
    this->position = vec;
    this->model = glm::translate(glm::mat4(1.0f), vec);
}

void Object3D::setModelMatrix(const glm::mat4 &mat) {
    this->model = mat;
}

const glm::mat4 &Object3D::getModelMatrix() const {
    return this->model;
}
