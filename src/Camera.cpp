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

#include "helper/QuatUtils.h"


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

    updateFrustum();
}

void Camera::updateFrustum() {
    /*
    auto m = projectionMatrix * viewMatrix;

    const auto &rowX = m[0];
    const auto &rowY = m[1];
    const auto &rowZ = m[2];
    const auto &rowW = m[3];

    frustumPlanes[0] = glm::normalize(rowW + rowX);
    frustumPlanes[1] = glm::normalize(rowW - rowX);
    frustumPlanes[2] = glm::normalize(rowW + rowY);
    frustumPlanes[3] = glm::normalize(rowW - rowY);
    frustumPlanes[4] = glm::normalize(rowW + rowZ);
    frustumPlanes[5] = glm::normalize(rowW - rowZ);


    for (auto &frustumPlane : frustumPlanes) {
        const auto normal = glm::vec3(frustumPlane.x, frustumPlane.y, frustumPlane.z);
        const auto length = glm::length(normal);

        frustumPlane = -frustumPlane / length;
    }
    */

/*
    frustumPlanes[NEARP  ] = glm::vec4(m[3][0] + m[2][1], m[3][1] + m[2][2], m[3][2] + m[2][3], m[3][3] + m[2][3]);
    frustumPlanes[FARP   ] = glm::vec4(m[3][0] - m[2][1], m[3][1] - m[2][2], m[3][2] - m[2][3], m[3][3] - m[2][3]);
    frustumPlanes[TOPP   ] = glm::vec4(m[3][0] - m[0][1], m[3][1] - m[0][2], m[3][2] - m[0][3], m[3][3] - m[0][3]);
    frustumPlanes[BOTTOMP] = glm::vec4(m[3][0] + m[1][1], m[3][1] + m[1][2], m[3][2] + m[1][3], m[3][3] + m[1][3]);
    frustumPlanes[LEFTP  ] = glm::vec4(m[3][0] + m[0][1], m[3][1] + m[0][2], m[3][2] + m[0][3], m[3][3] + m[0][3]);
    frustumPlanes[RIGHTP ] = glm::vec4(m[3][0] - m[0][1], m[3][1] - m[0][2], m[3][2] - m[0][3], m[3][3] - m[0][3]);

    for (auto &frustumPlane : frustumPlanes) {
        frustumPlane = frustumPlane / glm::length(glm::vec3(frustumPlane));
    }
*/

    const auto front = -this->front;
    //const auto right = glm::mat3(viewMatrix)[0];
    //const auto up = glm::mat3(viewMatrix)[1];

    const auto nC = this->position - front * zNear;
    const auto fC = this->position - front * 10.0f;
    glm::vec3 aux, point;

    frustumPlanes[NEARP  ]        = glm::vec4(-front, -glm::dot(-front, nC));
    frustumPlanes[FARP  ]         = glm::vec4( front, -glm::dot( front, fC));

    point = nC + up * npHeight;
    aux   = glm::normalize(glm::cross(glm::normalize(point - position), right));
    frustumPlanes[TOPP   ]        = glm::vec4(aux, -glm::dot(aux, point));

    point = nC - up * npHeight;
    aux   = glm::normalize(glm::cross(right, glm::normalize(point - position)));
    frustumPlanes[BOTTOMP]        = glm::vec4(aux, -glm::dot(aux, point));

    point = nC - right * npWidth;
    aux   = glm::normalize(glm::cross(glm::normalize(point - position), up));
    frustumPlanes[LEFTP  ]        = glm::vec4(aux, -glm::dot(aux, point));

    point = nC + right * npWidth;
    aux   = glm::normalize(glm::cross(up, glm::normalize(point - position)));
    frustumPlanes[RIGHTP ]        = glm::vec4(aux, -glm::dot(aux, point));

    auto &nw = npWidth;
    auto &fw = fpWidth;
    auto &nh = npHeight;
    auto &fh = fpHeight;

    auto ntl = nC + up * nh - right * nw;
    auto ntr = nC + up * nh + right * nw;
    auto nbl = nC - up * nh - right * nw;
    auto nbr = nC - up * nh + right * nw;
    auto ftl = fC + up * fh - right * fw;
    auto ftr = fC + up * fh + right * fw;
    auto fbl = fC - up * fh - right * fw;
    auto fbr = fC - up * fh + right * fw;
/*
    frustumPlanes[TOPP   ] = Plane(ntr,ntl,ftl);
    frustumPlanes[BOTTOMP] = Plane(nbl,nbr,fbr);
    frustumPlanes[LEFTP  ] = Plane(ntl,nbl,fbl);
    frustumPlanes[RIGHTP ] = Plane(nbr,ntr,fbr);
    frustumPlanes[NEARP  ] = Plane(ntl,ntr,nbr);
    frustumPlanes[FARP   ] = Plane(ftr,ftl,fbl);
*/
   debug_frustumPlanes = {
           ntl, ntr, nbr, nbl,     //Near
           ftr, ftl, fbl, fbr,     //Far
           nbl, nbr, fbr, fbl,     //Bottom
           ntr, ntl, ftl, ftr,     //Top
           ntl, nbl, fbl, ftl,     //Left
           nbr, ntr, ftr, fbr      //Right
   };
}


void Camera::screenSizeChanged(int width, int height) {
    this->screenRatio = (float)width/(float)height;
    this->projectionMatrix = glm::perspective(glm::radians(this->fov), screenRatio, zNear, zFar);
    this->viewport = glm::vec4(0,0, width, height);

    auto t = glm::tan(glm::radians(fov) * .5f);
    this->npHeight = zNear * t;
    this->npWidth  = npHeight * screenRatio;
    this->fpHeight = 7.0f * t;
    this->fpWidth  = fpHeight * screenRatio;

    tang = glm::tan(glm::radians(fov) / 2.0f);
    sphereFactorY = 1.0f/glm::cos(glm::radians(fov) / 2.0f);

    float anglex = glm::atan(tang*screenRatio);
    sphereFactorX = 1.0f/glm::cos(anglex);

    //this->npHeight /= 2;
    //this->npWidth /= 2;

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

void Camera::processMouseInput(std::chrono::duration<double, std::milli> delta, double xDelta, double yDelta) {
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
    //const auto front = this->front;
    //const auto right = glm::normalize(glm::cross(worldUp, front));
    //const auto up = glm::cross(front, right);

    glm::vec3 v = position - this->position;

    float az = glm::dot(v, front);
    float ax = glm::dot(v, right);
    float ay = glm::dot(v, up);

    float d1 = sphereFactorX * radius;
    float d2 = sphereFactorY * radius;

    float zz1 = az * tang * screenRatio;
    float zz2 = az * tang;

    if (az > zFar + radius || az < zNear-radius)
        return OUTSIDE;
    if (ax > zz1+d1 || ax < -zz1-d1)
        return OUTSIDE;
    if (ay > zz2+d2 || ay < -zz2-d2)
        return OUTSIDE;

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
