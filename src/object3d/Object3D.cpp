//
// Created by raphael on 13.03.18.
//

#include "Object3D.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>
void Object3D::render(const Camera &camera) {
    this->shader->use();
    this->shader->setUniform("model", this->model);
    this->shader->setUniform("view", camera.getViewMatrix());
    this->shader->setUniform("projection", camera.getProjectionMatrix());
    this->shader->setUniformIfNeeded("camera_position", camera.getPosition());

    this->draw();
}

Object3D::Object3D(const std::shared_ptr<Shader> &shader) {
    this->shader = shader;
}

void Object3D::rotate(float angle, const glm::vec3 &vec) {
    this->model = glm::rotate(this->model, glm::radians(angle), vec);
}

void Object3D::translate(const glm::vec3 &vec) {
    this->model = glm::translate(this->model, vec);
}

void Object3D::rotate(const glm::quat &quat) {
    this->model = this->model * glm::toMat4(quat);
}

void Object3D::setOrigin(const glm::vec3 &vec) {
    this->model = glm::translate(glm::mat4(1.0f), vec);
}