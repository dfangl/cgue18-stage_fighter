//
// Created by raphael on 13.03.18.
//

#include "Object3D.h"

#include "../Scene.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>

void Object3D::render(Scene *scene) {
    this->shader->use();
    this->shader->setUniform("model", this->model);
    this->shader->setUniform("view", scene->getCamera().getViewMatrix());
    this->shader->setUniform("projection", scene->getCamera().getProjectionMatrix());

    this->shader->setUniformIfNeeded("camera_position", scene->getCamera().getPosition());
    this->shader->setUniformIfNeeded("screenGamma", scene->gamma);

    // TODO: support multiple lights with #DEFINE_MAX_LIGHTS
    if (scene->areLightsDirty()) {
        this->shader->setUniform("light.position", scene->getLights()[0].position);
        this->shader->setUniform("light.diffuse", scene->getLights()[0].diffuse);
        this->shader->setUniform("light.ambient", scene->getLights()[0].ambient);
        this->shader->setUniform("light.specular", scene->getLights()[0].specular);
        this->shader->setUniform("light.power", scene->getLights()[0].power);
    }

    this->draw();
}

Object3D::Object3D(const glm::vec3 &position, float boundingSphereRadius, const std::shared_ptr<Shader> &shader) {
    this->shader = shader;
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