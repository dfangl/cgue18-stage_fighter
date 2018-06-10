//
// Created by Raphael on 27.04.2018.
//

#ifndef STAGE_FIGHTER_LIGHT_H
#define STAGE_FIGHTER_LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Light {
    Light(const glm::vec3 &pos, const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular, const float power = 40.0f) :
        position(pos), ambient(ambient), diffuse(diffuse), specular(specular), power(power) {
    }

    glm::vec3 position;
    float __PADDING_1 = 0;

    glm::vec3 ambient;
    float __PADDING_2 = 0;

    glm::vec3 diffuse;
    float __PADDING_3 = 0;

    glm::vec3 specular;
    float __PADDING_4 = 0;

    float power;
    float __PADDING_5[3] = {0,0,0};
};

#endif //STAGE_FIGHTER_LIGHT_H