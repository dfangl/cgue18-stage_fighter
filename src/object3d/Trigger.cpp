//
// Created by Raphael on 16.06.2018.
//

#include "Trigger.h"

Trigger::Trigger(const glm::vec3 &position, float radius, kaguya::LuaTable env)
    : position(position), radius(radius), environment(env) {}

void Trigger::update(const glm::vec3 &playerPos) {
    const auto distance = glm::distance(position, playerPos);

    if (distance < radius && !triggered) {
        triggered = true;
        environment["entered"]();
    } else if (distance > radius && triggered) {
        triggered = false;
        environment["left"]();
    }

}
