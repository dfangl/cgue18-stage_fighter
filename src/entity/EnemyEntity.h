//
// Created by raphael on 26.04.18.
//

#ifndef STAGE_FIGHTER_ENEMYENTITY_H
#define STAGE_FIGHTER_ENEMYENTITY_H

#include <string>

#include <glm/glm.hpp>

#include "Entity.h"

#include "../object3d/BulletObject.h"
#include "../object3d/Model3DObject.h"
#include "../BulletUniverse.h"

class EnemyEntity : public Entity, public BulletObject, public Model3DObject {

private:
    std::shared_ptr<BulletUniverse> world;
    double lastSpawnTime;
    double spawnSpeed;

public:
    EnemyEntity(std::string name, int health, int spawnTime, const btVector3 &pos, const btQuaternion &rot, std::string model, float mass,
                btCollisionShape *hitbox, const std::shared_ptr<BulletUniverse> &world);
    ~EnemyEntity();

    void think(Level *level, std::chrono::duration<double, std::milli> delta) override;

    void collideWith(BulletObject *other) override;

    void setEntityPosition(const glm::vec3 &vec, const glm::quat &rot) override;

    void think(std::chrono::duration<double, std::milli> delta) override;

    void render(const Camera &camera) override;
};


#endif //STAGE_FIGHTER_ENEMYENTITY_H
