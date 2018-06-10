//
// Created by Raphael on 27.04.2018.
//

#ifndef STAGE_FIGHTER_LIGHT_H
#define STAGE_FIGHTER_LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Light {
    Light(const glm::vec3 &pos, const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular, const float power = 40.0f) :
        position(pos, 0.0f), ambient(ambient, 0.0f), diffuse(diffuse, 0.0f), specular(specular, 0.0f), power(power) {
    }

    glm::vec4 position;
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float power;

    float __PADDING__[] = {0,0,0};
};

#endif //STAGE_FIGHTER_LIGHT_H