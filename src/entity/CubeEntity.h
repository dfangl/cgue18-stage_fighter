//
// Created by Raphael on 17.03.2018.
//

#ifndef STAGE_FIGHTER_CUBEENTITY_H
#define STAGE_FIGHTER_CUBEENTITY_H

#include "../object3d/Cube.h"
#include "../object3d/BulletObject.h"
#include "../BulletUniverse.h"
#include "Entity.h"

class CubeEntity : public Entity, public BulletObject, public Cube {

    std::shared_ptr<BulletUniverse> world;

public:
    CubeEntity(const glm::vec3 &pos, const std::shared_ptr<Texture> &texture, std::shared_ptr<BulletUniverse> world);
    virtual ~CubeEntity();

    void think(std::chrono::duration<double, std::milli> delta) override;

    void setEntityPosition(const glm::vec3 &vec, const glm::quat &rot) override;

    virtual void render(const Camera &camera);
};


#endif //STAGE_FIGHTER_CUBEENTITY_H
