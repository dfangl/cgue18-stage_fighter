//
// Created by raphael on 13.03.18.
//

#include "Object3D.h"

void Object3D::render() {
    this->shader->use();
    this->draw();
}

Object3D::Object3D(const std::shared_ptr<Shader> &shader) {
    this->shader = shader;
}
