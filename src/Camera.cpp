//
// Created by raphael on 15.03.18.
//

#include "Camera.h"

#include <glm/glm.hpp>
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

    //this->logger->info("{}\tviewMatrix->position = ({},{},{});", static_cast<void*>(this), position.x, position.y, position.z);
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
    this->yaw   += xDelta * mouseSensitivity;
    this->pitch -= yDelta * mouseSensitivity;

    if(this->pitch >  89.0f) this->pitch =  89.0f;
    if(this->pitch < -89.0f) this->pitch = -89.0f;

    this->update();
}

void Camera::update(const glm::vec3 position) {
    //this->logger->info("{}\tupdate({},{},{});", static_cast<void*>(this), position.x, position.y, position.z);
    this->position = position;
    this->update();
}

void Camera::lookAt(const glm::vec3 &object) {
    //TODO: This doesn't work ...

    const auto &direction = glm::normalize(
            glm::vec3(position.x - object.x, position.y - object.y, position.z - object.z)
    );

    this->pitch = static_cast<float>(M_PI * asinf(direction.y));
    this->yaw = static_cast<float>(M_PI * atan2f(direction.x, direction.z));
}
