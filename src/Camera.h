//
// Created by raphael on 15.03.18.
//

#ifndef STAGE_FIGHTER_CAMERA_H
#define STAGE_FIGHTER_CAMERA_H

#include <functional>
#include <chrono>

#include <glm/glm.hpp>

#include "helper/Logger.h"

/**
 * Basic Camera class which does contain the viewMatrix, projectionMatrix and the viewport size
 * It also supports movement with the functions processKeyInput and processMouseInput
 */
class Camera /*: public Logger*/ {

private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;
    float fov, zNear, zFar;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::vec4 viewport;

    void update();

public:
    enum Movement {
        FORWARD, BACKWARD, LEFT, RIGHT
    };

    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float fov, float width, float height, float zNear = 0.1f, float zFar = 100.0f);

    void processKeyInput(std::chrono::duration<double, std::milli> delta, Movement movement);
    void processMouseInput(std::chrono::duration<double, std::milli> delta, double xDelta, double yDelta);

    void screenSizeChanged(int width, int height);

    glm::mat4 getViewMatrix() const { return this->viewMatrix; }
    glm::mat4 getProjectionMatrix() const { return this->projectionMatrix; }
    glm::vec4 getViewPort() const { return this->viewport; }

    float mouseSensitivity = 0.1f;
    float keySensitivity = 0.1f;
    bool enableUpdate = true;

    void update(const glm::vec3 position);

    // TODO: borket ...
    void lookAt(const glm::vec3 &object);

    glm::vec3 project(const glm::vec3 &object) const;

    glm::vec3 getPosition() const { return this->position; }
    glm::vec3 getFront() const { return this->front; }
    glm::vec3 getRight() const { return this->right; }
    float getPitch() const { return this->pitch; }
    float getYaw() const { return this->yaw; }
};


#endif //STAGE_FIGHTER_CAMERA_H
