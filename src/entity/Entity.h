//
// Created by Raphael on 17.03.2018.
//

#ifndef STAGE_FIGHTER_ENTITY_H
#define STAGE_FIGHTER_ENTITY_H

#include <chrono>

class Entity {

public:
    virtual ~Entity() = default;

    virtual void setEntityPosition(const glm::vec3 &vec) = 0;
    virtual void think(std::chrono::duration<double, std::milli> delta) = 0;

};


#endif //STAGE_FIGHTER_ENTITY_H
