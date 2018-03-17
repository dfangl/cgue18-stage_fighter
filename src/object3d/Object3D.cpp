//
// Created by raphael on 13.03.18.
//

#include "Object3D.h"
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
void Object3D::render(const Camera &camera) {
    this->shader->use();                                                        if( glGetError() != GL_NO_ERROR ) std::cout << "[Object3D] this->shader-use()" << std::endl;
    this->shader->setUniform("model", this->model);                             if( glGetError() != GL_NO_ERROR ) std::cout << "[Object3D] this->shader set model" << std::endl;
    this->shader->setUniform("view", camera.getViewMatrix());                   if( glGetError() != GL_NO_ERROR ) std::cout << "[Object3D] this->shader set view" << std::endl;
    this->shader->setUniform("projection", camera.getProjectionMatrix());       if( glGetError() != GL_NO_ERROR ) std::cout << "[Object3D] this->shader set projection" << std::endl;

    this->draw();
}

Object3D::Object3D(const std::shared_ptr<Shader> &shader) {
    this->shader = shader;
}

void Object3D::rotate(float angle, const glm::vec3 &vec) {
    this->model = glm::rotate(this->model, angle, vec);
}

void Object3D::translate(const glm::vec3 &vec) {
    this->model = glm::translate(this->model, vec);
}
