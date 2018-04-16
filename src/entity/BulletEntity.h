//
// Created by raphael on 16.04.18.
//

#ifndef STAGE_FIGHTER_BULLETENTITY_H
#define STAGE_FIGHTER_BULLETENTITY_H


#include "../BulletUniverse.h"
#include "../object3d/BulletObject.h"
#include "../object3d/Model3DObject.h"

#include "Entity.h"

class BulletEntity : public Entity, public BulletObject, public Model3DObject {

private:
    std::shared_ptr<BulletUniverse> world;
    float speed = 5.0f;
    glm::vec3 direction;

public:
    BulletEntity(const btVector3 &pos, const btVector3 &target, std::shared_ptr<BulletUniverse> &world);
    ~BulletEntity();

    void think(Level *level, std::chrono::duration<double, std::milli> delta) override;

    void collideWith(BulletObject *other) override;

    void setEntityPosition(const glm::vec3 &vec, const glm::quat &rot) override;

    void think(std::chrono::duration<double, std::milli> delta) override;

    void render(const Camera &camera) override;
};


#endif //STAGE_FIGHTER_BULLETENTITY_H
