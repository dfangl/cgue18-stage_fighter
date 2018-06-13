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
    this->shader->use();
    this->shader->setUniformIfNeeded("model", this->model);
    this->shader->setUniform("view", scene->getCamera().getViewMatrix());
    this->shader->setUniform("projection", scene->getCamera().getProjectionMatrix());

    this->shader->setUniformIfNeeded("camera_position", scene->getCamera().getPosition());
    this->shader->setUniformIfNeeded("screenGamma", scene->gamma);

    if (scene->areLightsDirty() || !shader->hasLightDataSet) {

        auto lLoc = shader->getLocation("light[0]");
        const auto lsLoc = shader->getLocation("lights");

        // Can someone tell me why?
        if (lLoc == -1 && lsLoc > -1) {
            spdlog::get("console")->warn("lights defined but unable to query light[] location! Assuming light = lights+1!");
            lLoc = lsLoc + 1;
        }

        if (lLoc != -1) {
            const auto &lights = scene->getLights();
            const size_t border = std::min(MAX_LIGHTS, lights.size());
            shader->setUniform(lsLoc, (int)lights.size());

            for(size_t i = 0; i < border; i++) {
                const auto Loc = lLoc + (i * 5 /*uniform slots*/);

                shader->setUniform(static_cast<GLint>(Loc + 0), lights[i].position);
                shader->setUniform(static_cast<GLint>(Loc + 1), lights[i].ambient);
                shader->setUniform(static_cast<GLint>(Loc + 2), lights[i].diffuse);
                shader->setUniform(static_cast<GLint>(Loc + 3), lights[i].specular);
                shader->setUniform(static_cast<GLint>(Loc + 4), lights[i].power);
            }
        }

        // this->shader->setUniform("light.position", scene->getLights()[0].position);
        // this->shader->setUniform("light.diffuse", scene->getLights()[0].diffuse);
        // this->shader->setUniform("light.ambient", scene->getLights()[0].ambient);
        // this->shader->setUniform("light.specular", scene->getLights()[0].specular);
        // this->shader->setUniform("light.power", scene->getLights()[0].power);

        this->shader->hasLightDataSet = true;
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

void Object3D::setModelMatrix(const glm::mat4 &mat) {
    this->model = mat;
}

const glm::mat4 &Object3D::getModelMatrix() const {
    return this->model;
}
