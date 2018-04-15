//
// Created by Raphael on 17.03.2018.
//

#ifndef STAGE_FIGHTER_ENTITY_H
#define STAGE_FIGHTER_ENTITY_H

#include <chrono>
#include <glm/glm.hpp>

#include "../object3d/Object3D.h"

class Entity : public Object3DAbstract {

protected:
    glm::vec3 position;

public:
    virtual ~Entity() = default;

    virtual void setEntityPosition(const glm::vec3 &vec, const glm::quat &rot) = 0;
    virtual glm::vec3 getEntityPosition() const { return position; }
    virtual void think(std::chrono::duration<double, std::milli> delta) = 0;

    virtual void render(const Camera &camera) = 0;

};


#endif //STAGE_FIGHTER_ENTITY_H
