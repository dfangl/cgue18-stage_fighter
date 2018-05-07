//
// Created by raphael on 15.03.18.
//

#include <algorithm>

#define _USE_MATH_DEFINES
#include <cmath>

#include "Camera.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#include "helper/QuatUtils.h"


Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float fov, float width, float height, float zNear, float zFar) {
    this->position = position;
    this->worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;
    this->fov = fov;

    this->zNear = zNear;
    this->zFar = zFar;

    this->projectionMatrix = glm::perspective(glm::radians(fov), width/height, zNear, zFar);
    this->viewport = glm::vec4(0,0, width, height);
    this->update();
}

void Camera::update() {
    glm::vec3 front;
    front.x = static_cast<float>(cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)));
    front.y = static_cast<float>(sin(glm::radians(this->pitch)));
    front.z = static_cast<float>(sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)));
    this->front = glm::normalize(front);

    this->right = glm::normalize(glm::cross(this->front, this->worldUp));
    this->up = glm::normalize(glm::cross(this->right, this->front));

    //this->logger->info("{}\tviewMatrix->position = ({},{},{});", static_cast<void*>(this), position.x, position.y, position.z);
    this->viewMatrix = glm::lookAt(this->position, position + front, up);
}

void Camera::screenSizeChanged(int width, int height) {
    this->projectionMatrix = glm::perspective(glm::radians(this->fov), (float)width/(float)height, zNear, zFar);
    this->viewport = glm::vec4(0,0, width, height);
    this->update();
}

void Camera::processKeyInput(std::chrono::duration<double, std::milli> delta, Camera::Movement movement) {
    if (!enableUpdate)
        return;

    const auto velocity = static_cast<float>(this->keySensitivity * delta.count());

    switch(movement) {
        case FORWARD : this->position += this->front * velocity; update(); break;
        case LEFT    : this->position -= this->right * velocity; update(); break;
        case RIGHT   : this->position -= this->front * velocity; update(); break;
        case BACKWARD: this->position += this->right * velocity; update(); break;
        default: /*Nothing*/ ;
    }
}

void Camera::processMouseInput(std::chrono::duration<double, std::milli> delta, double xDelta, double yDelta) {
    if (!enableUpdate)
        return;

    if (xDelta > 100 && yDelta > 100) {
        spdlog::get("console")->critical("Mouse moved too fast, discarding events!");
        return;
    }

    this->yaw   += xDelta * mouseSensitivity;
    this->pitch -= yDelta * mouseSensitivity;

    if(this->pitch >  89.0f) this->pitch =  89.0f;
    if(this->pitch < -89.0f) this->pitch = -89.0f;
    this->update();
}

void Camera::update(const glm::vec3 position) {
    this->position = position;
    this->update();
}

void Camera::lookAt(const glm::vec3 &object) {
    const auto direction = object - position;
    const auto q = Quat::rotateTo(direction);

    glm::vec3 newUp = q * glm::vec3(0.0f, 1.0f, 0.0f);
    const auto rot2 = Quat::rotation(newUp, this->up);

    const auto rotation = rot2 * q;

    this->pitch = glm::degrees(glm::pitch(rotation));
    this->yaw   = -glm::degrees(glm::yaw(rotation));

    if(this->pitch >  89.0f) this->pitch =  89.0f;
    if(this->pitch < -89.0f) this->pitch = -89.0f;
    this->update();
}

glm::vec3 Camera::project(const glm::vec3 &obj) const {
    return glm::project(obj, viewMatrix, projectionMatrix, viewport);
}
