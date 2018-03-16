//
// Created by raphael on 15.03.18.
//

#ifndef STAGE_FIGHTER_CAMERA_H
#define STAGE_FIGHTER_CAMERA_H

#include <glm/glm.hpp>
#include <functional>
#include <chrono>

class Camera {

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

    float mouseSensitivity = 0.01f;
    float keySensitivity = 0.001f;

    void update();

public:
    enum Movement {
        FORWARD, BACKWARD, LEFT, RIGHT
    };

    Camera() = default;
    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float fov, float width, float height, float zNear = 0.1f, float zFar = 100.0f);

    void processKeyInput(std::chrono::duration<double, std::milli> delta, Movement movement);
    void processMouseInput(std::chrono::duration<double, std::milli> delta, double xDelta, double yDelta);

    void screenSizeChanged(int width, int height);

    glm::mat4 getViewMatrix() const { return this->viewMatrix; }
    glm::mat4 getProjectionMatrix() const { return this->projectionMatrix; }

};


#endif //STAGE_FIGHTER_CAMERA_H
