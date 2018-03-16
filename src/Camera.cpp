//
// Created by raphael on 15.03.18.
//

#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float fov, float width, float height, float zNear, float zFar) {
    this->position = position;
    this->worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;
    this->fov = fov;

    this->projectionMatrix = glm::perspective(glm::radians(fov), width/height, zNear, zFar);
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

    this->viewMatrix = glm::lookAt(this->position, position + front, up);
}

void Camera::screenSizeChanged(int width, int height) {
    this->projectionMatrix = glm::perspective(glm::radians(this->fov), (float)width/(float)height, zNear, zFar);
}

void Camera::processKeyInput(std::chrono::duration<double, std::milli> delta, Camera::Movement movement) {
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
    auto mouseMovement = static_cast<const float>(delta.count() * mouseSensitivity);

    this->yaw   += xDelta * mouseMovement;
    this->pitch -= yDelta * mouseMovement;

    if(this->pitch >  89.0f) this->pitch =  89.0f;
    if(this->pitch < -89.0f) this->pitch = -89.0f;

    this->update();
}
