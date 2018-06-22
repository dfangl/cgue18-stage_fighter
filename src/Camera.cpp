//
// Created by raphael on 15.03.18.
//

#include <algorithm>

#include "Camera.h"

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_access.hpp>

#include "helper/QuatUtils.h"
#include "helper/CompilerMacros.h"


Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float fov, float width, float height, float zNear, float zFar) {
    this->position = position;
    this->worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;
    this->fov = fov;

    this->zNear = zNear;
    this->zFar = zFar;

    this->screenSizeChanged(static_cast<int>(width), static_cast<int>(height));
}

void Camera::update() {
    glm::vec3 front;
    front.x = static_cast<float>(cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)));
    front.y = static_cast<float>(sin(glm::radians(this->pitch)));
    front.z = static_cast<float>(sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)));
    this->front = glm::normalize(front);

    this->right = glm::normalize(glm::cross(this->worldUp, this->front));
    this->up = glm::normalize(glm::cross(this->front, this->right));

    //this->logger->info("{}\tviewMatrix->position = ({},{},{});", static_cast<void*>(this), position.x, position.y, position.z);
    this->viewMatrix = glm::lookAt(this->position, position + front, worldUp);

    //updateFrustum();
}

void Camera::updateFrustum() {
    auto m = projectionMatrix * viewMatrix;

    const auto &col1 = glm::column(m, 0);
    const auto &col2 = glm::column(m, 1);
    const auto &col3 = glm::column(m, 2);
    const auto &col4 = glm::column(m, 3);

    frustumPlanes[0] = glm::normalize(col4 + col1);
    frustumPlanes[1] = glm::normalize(col4 - col1);
    frustumPlanes[2] = glm::normalize(col4 + col2);
    frustumPlanes[3] = glm::normalize(col4 - col2);
    frustumPlanes[4] = glm::normalize(col4 + col3);
    frustumPlanes[5] = glm::normalize(col4 - col3);


    for (auto &frustumPlane : frustumPlanes) {
        const auto normal = glm::vec3(frustumPlane);
        const auto length = glm::length(normal);

        frustumPlane = (frustumPlane / length);
    }

}


void Camera::screenSizeChanged(int width, int height) {
    this->screenRatio = (float)width/(float)height;
    this->projectionMatrix = glm::perspective(glm::radians(this->fov), screenRatio, zNear, zFar);
    this->viewport = glm::vec4(0,0, width, height);

    auto t = glm::tan(glm::radians(fov) * .5f);
    this->npHeight = zNear * t;
    this->npWidth  = npHeight * screenRatio;
    this->fpHeight = zFar * t;
    this->fpWidth  = fpHeight * screenRatio;

    tang = glm::tan(glm::radians(fov) * 0.5f);
    sphereFactorY = 1.0f/glm::cos(glm::radians(fov) * 0.5f);

    float anglex = glm::atan(tang*screenRatio);
    sphereFactorX = 1.0f/glm::cos(anglex);

    this->npHeight /= 2;
    this->npWidth /= 2;

    this->update();
}

void Camera::processKeyInput(std::chrono::duration<double, std::milli> delta, Camera::Movement movement) {
    if (!enableUpdate)
        return;

    const auto velocity = static_cast<float>(this->keySensitivity * delta.count());

    switch(movement) {
        case FORWARD : this->position += this->front * velocity; update(); break;
        case LEFT    : this->position += this->right * velocity; update(); break;
        case RIGHT   : this->position -= this->front * velocity; update(); break;
        case BACKWARD: this->position -= this->right * velocity; update(); break;
        default: /*Nothing*/ ;
    }
}

void Camera::processMouseInput(std::chrono::duration<double, std::milli> UNUSED(delta), double xDelta, double yDelta) {
    if (!enableUpdate)
        return;

    // Don't do anything is mouse did not move
    if (xDelta == 0 && yDelta == 0)
        return;

    // Dirty fix so lookAt does work
    //if (xDelta > 100 || yDelta > 100) {
    //    spdlog::get("console")->critical("Mouse moved too fast, discarding events!");
    //    return;
    //}

    this->yaw   += xDelta * mouseSensitivity;
    this->pitch -= yDelta * mouseSensitivity;

    if(this->pitch >  89.0f) this->pitch =  89.0f;
    if(this->pitch < -89.0f) this->pitch = -89.0f;
    this->update();
}

void Camera::update(const glm::vec3 position) {
    // Don't refresh is delta position is not big enough
    auto eps = this->position - position;
    if ((eps.x < 0.0001 && eps.y < 0.0001 && eps.z < 0.0001) && (-eps.x < 0.0001 && -eps.y < 0.0001 && -eps.z < 0.0001))
        return;

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

Camera::FrustumLocation Camera::isInFrustum(const glm::vec3 &position, float radius) {
    Camera::FrustumLocation result = INSIDE;
    //const auto front = glm::normalize(-this->front);
    //const auto right = glm::normalize(glm::cross(worldUp, front));
    //const auto up = glm::cross(front, right);

    glm::vec3 v = position - this->position;

    float az = glm::dot(v, front);
    float ax = glm::dot(v, right);
    float ay = glm::dot(v, up);

    float d1 = sphereFactorX * radius;
    float d2 = sphereFactorY * radius;

    float auxX = az * tang * screenRatio + d1;
    float auxY = az * tang               + d2;

    if (az > (zFar + radius) || az < (zNear-radius)) {
        return OUTSIDE;
    }
    if (ax > auxX || ax < -auxX) {
        return OUTSIDE;
    }
    if (ay > auxY || ay < -auxY) {
        return OUTSIDE;
    }

    /*
    if (az > zFar - radius || az < zNear+radius)
        return INTERSECT;
    if (ay > zz2-d2 || ay < -zz2+d2)
        return INTERSECT;
    if (ax > zz1-d1 || ax < -zz1+d1)
        return INTERSECT;
    */

    return result;
}
