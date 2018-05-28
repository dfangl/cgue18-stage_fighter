//
// Created by Raphael on 17.03.2018.
//

#ifndef STAGE_FIGHTER_ENTITY_H
#define STAGE_FIGHTER_ENTITY_H

#include <string>
#include <chrono>
#include <glm/glm.hpp>

#include "../object3d/Object3D.h"
class Level; //#include "../level/Level.h"

class Entity : public Object3DAbstract {

protected:
    std::string name;

    int health, maxHealth;
    double lastDmgTime;
    double dmgTimeout = 100; // ms

public:
    virtual ~Entity() = default;

    virtual void setPosition(const glm::vec3 &vec, const glm::quat &rot) = 0;
    virtual float getBoundingSphereRadius() = 0;
    virtual const glm::vec3 &getPosition() const = 0;

    virtual void think(Level *level, std::chrono::duration<double, std::milli> delta) { think(delta); }
    virtual void think(std::chrono::duration<double, std::milli> delta) = 0;

    virtual void render(Scene *scene) = 0;

    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }

    void receiveDamage(int points) {  if (lastDmgTime <= 0.0) this->health = std::max(this->health - points, 0); }

    bool mustBeKilled = false;
};


#endif //STAGE_FIGHTER_ENTITY_H
