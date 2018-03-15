//
// Created by raphael on 13.03.18.
//

#include "Object3D.h"
#include <glm/gtc/matrix_transform.hpp>

void Object3D::render() {
    this->shader->use();
    this->shader->setUniform("transform", this->transform);
    this->draw();
}

Object3D::Object3D(const std::shared_ptr<Shader> &shader) {
    this->shader = shader;
}

void Object3D::rotate(float angle, const glm::vec3 &vec) {
    this->transform = glm::rotate(this->transform, angle, vec);
}

void Object3D::translate(const glm::vec3 &vec) {
    this->transform = glm::translate(this->transform, vec);
}
