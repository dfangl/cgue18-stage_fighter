//
// Created by Raphael on 17.03.2018.
//

#ifndef STAGE_FIGHTER_CUBEENTITY_H
#define STAGE_FIGHTER_CUBEENTITY_H

#include "../object3d/Cube.h"
#include "../object3d/BulletObject.h"

#include "../BulletUniverse.h"
#include "../Scene.h"

#include "Entity.h"

class CubeEntity : public Entity, public BulletObject, public Cube {

    std::shared_ptr<BulletUniverse> world;
    float spawnSpeed = 2000; //ms
    double lastSpawnTime = spawnSpeed;

public:
    CubeEntity(const glm::vec3 &pos, const std::shared_ptr<Texture> &texture, std::shared_ptr<BulletUniverse> world);
    virtual ~CubeEntity();

    void think(Level *level, std::chrono::duration<double, std::milli> delta) override;

    void think(std::chrono::duration<double, std::milli> delta) override;

    void setEntityPosition(const glm::vec3 &vec, const glm::quat &rot) override;

    void render(Scene *scene) override;

    void collideWith(BulletObject *other) override;
};


#endif //STAGE_FIGHTER_CUBEENTITY_H
