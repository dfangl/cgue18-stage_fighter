//
// Created by Raphael on 27.04.2018.
//

#ifndef STAGE_FIGHTER_LIGHT_H
#define STAGE_FIGHTER_LIGHT_H

#endif //STAGE_FIGHTER_LIGHT_H

#include <glm/glm.hpp>

class Light {
protected:
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

public:
    Light(const glm::vec3 &pos, const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular);

    glm::vec3 getPosition() const { return position; }
    glm::vec3 getDiffuse() const { return diffuse; }
    glm::vec3 getAmbient() const { return ambient; }
    glm::vec3 getSpecular() const { return specular; }
};