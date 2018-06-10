//
// Created by raphael on 07.05.18.
//

#ifndef STAGE_FIGHTER_QUATUTILS_H
#define STAGE_FIGHTER_QUATUTILS_H

// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>

class Quat {

public:
    static glm::quat rotation(const glm::vec3 &start, const glm::vec3 &end) {
        auto sVec = glm::normalize(start);
        auto eVec = glm::normalize(end);

        float cosTheta = glm::dot(sVec, eVec);
        glm::vec3 rotationAxis;

        // special case when vectors in opposite directions, there is no "ideal" rotation axis
        // So guess one; any will do as long as it's perpendicular to start
        if (cosTheta < -1 + 0.001f){
            rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);

            // they were parallel, try again!
            if (glm::length2(rotationAxis) < 0.01 )
                rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

            rotationAxis = normalize(rotationAxis);
            return glm::angleAxis(glm::radians(180.0f), rotationAxis);
        } else {
            rotationAxis = glm::cross(sVec, eVec);

            float s = glm::sqrt( (1+cosTheta)*2.0f );
            float invs = 1 / s;

            return glm::quat(
                    s * 0.5f,
                    rotationAxis.x * invs,
                    rotationAxis.y * invs,
                    rotationAxis.z * invs
            );
        }

    }

    static glm::quat rotateTo(const glm::vec3 &direction) {
        return Quat::rotation(glm::vec3(0.0f, 0.0f, 1.0f), direction);
    }


    //https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
    static glm::quat toQuaternion(float pitch, float roll, float yaw) {
        // Abbreviations for the various angular functions
        float cy = glm::cos(glm::radians(yaw  ) * 0.5f);
        float sy = glm::sin(glm::radians(yaw  ) * 0.5f);
        float cr = glm::cos(glm::radians(roll ) * 0.5f);
        float sr = glm::sin(glm::radians(roll ) * 0.5f);
        float cp = glm::cos(glm::radians(pitch) * 0.5f);
        float sp = glm::sin(glm::radians(pitch) * 0.5f);

        return glm::quat(
                sy * cr * cp - cy * sr * sp, // W
                cy * cr * cp + sy * sr * sp, // X
                cy * sr * cp - sy * cr * sp, // Y
                cy * cr * sp + sy * sr * cp  // Z
        );
    }

};


#endif //STAGE_FIGHTER_QUATUTILS_H
