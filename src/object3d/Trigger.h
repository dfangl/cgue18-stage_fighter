//
// Created by Raphael on 16.06.2018.
//

#ifndef STAGE_FIGHTER_TRIGGER_H
#define STAGE_FIGHTER_TRIGGER_H

#include <glm/glm.hpp>
#include <kaguya/kaguya.hpp>

class Trigger {

private:
    glm::vec3 position;
    float radius;

    kaguya::LuaTable environment;

    bool triggered = false;
public:
    Trigger(const glm::vec3 &position, float radius, kaguya::LuaTable env);

    void update(const glm::vec3 &playerPos);

};


#endif //STAGE_FIGHTER_TRIGGER_H
