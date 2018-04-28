//
// Created by Raphael on 27.04.2018.
//

#ifndef STAGE_FIGHTER_LIGHT_H
#define STAGE_FIGHTER_LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Light {

public:
    Light(const glm::vec3 &pos, const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular) :
        position(pos), ambient(ambient), diffuse(diffuse), specular(specular) {
    }

    float power = 40.0f;
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

#endif //STAGE_FIGHTER_LIGHT_H