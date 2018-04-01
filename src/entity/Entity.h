//
// Created by Raphael on 17.03.2018.
//

#ifndef STAGE_FIGHTER_ENTITY_H
#define STAGE_FIGHTER_ENTITY_H

#include <chrono>
#include <glm/glm.hpp>

#include "../object3d/Object3D.h"

class Entity : public Object3DAbstract {

public:
    virtual ~Entity() = default;

    virtual void setEntityPosition(const glm::vec3 &vec, const glm::quat &rot) = 0;
    virtual void think(std::chrono::duration<double, std::milli> delta) = 0;

    virtual void render(const Camera &camera) = 0;

};


#endif //STAGE_FIGHTER_ENTITY_H
