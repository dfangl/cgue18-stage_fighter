//
// Created by raphael on 15.03.18.
//

#ifndef STAGE_FIGHTER_CAMERA_H
#define STAGE_FIGHTER_CAMERA_H

#include <functional>
#include <chrono>
#include <vector>

#include <glm/glm.hpp>

#include "helper/Logger.h"

/**
 * Basic Camera class which does contain the viewMatrix, projectionMatrix and the viewport size
 * It also supports movement with the functions processKeyInput and processMouseInput
 */
class Camera /*: public Logger*/ {

public:
    enum PlanePosition {
        NEARP, FARP, LEFTP, RIGHTP, TOPP, BOTTOMP
    };

    enum FrustumLocation {
        INSIDE, OUTSIDE, INTERSECT
    };

    struct Plane {
        glm::vec3 normal;
        float d;

        float distance(const glm::vec3 &point) {
            return d + glm::dot(point, normal);
        }
    };

    std::vector<glm::vec3> debug_frustumPlanes;

private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;
    float fov, zNear, zFar;
    float screenRatio;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::vec4 viewport;

    glm::vec4 frustumPlanes[6];
    float fpHeight, fpWidth;
    float npHeight, npWidth;

    void update();
    void updateFrustum();

    float sphereFactorY, sphereFactorX, tang;

public:
    enum Movement {
        FORWARD, BACKWARD, LEFT, RIGHT
    };

    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float fov, float width, float height, float zNear = 0.1f, float zFar = 100.0f);
    Camera(Camera&) = delete;
    Camera &operator=(Camera&) = delete;

    void processKeyInput(std::chrono::duration<double, std::milli> delta, Movement movement);
    void processMouseInput(std::chrono::duration<double, std::milli> delta, double xDelta, double yDelta);

    void screenSizeChanged(int width, int height);

    const glm::mat4 &getViewMatrix() const { return this->viewMatrix; }
    const glm::mat4 &getProjectionMatrix() const { return this->projectionMatrix; }
    const glm::vec4 &getViewPort() const { return this->viewport; }

    float mouseSensitivity = 0.1f;
    float keySensitivity = 0.1f;
    bool enableUpdate = true;

    void update(const glm::vec3 position);

    // TODO: borket ...
    void lookAt(const glm::vec3 &object);

    glm::vec3 project(const glm::vec3 &object) const;

    const glm::vec3 &getPosition() const { return this->position; }
    const glm::vec3 &getFront() const { return this->front; }
    const glm::vec3 &getRight() const { return this->right; }
    float getPitch() const { return this->pitch; }
    float getYaw() const { return this->yaw; }

    glm::vec4* getFrustumPlanes() { return this->frustumPlanes; }
    FrustumLocation isInFrustum(const glm::vec3 &position, float radius);
};


#endif //STAGE_FIGHTER_CAMERA_H
