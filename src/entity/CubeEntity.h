//
// Created by Raphael on 17.03.2018.
//

#ifndef STAGE_FIGHTER_CUBEENTITY_H
#define STAGE_FIGHTER_CUBEENTITY_H

#include "../object3d/Cube.h"
#include "../object3d/BulletObject.h"
#include "../BulletUniverse.h"
#include "Entity.h"

//Entity is no class-name ???
class CubeEntity : public Cube, public BulletObject /*, public Entity */ {

    std::shared_ptr<BulletUniverse> world;

public:
    CubeEntity(const glm::vec3 &pos, const std::shared_ptr<Texture> &texture, std::shared_ptr<BulletUniverse> world);
    ~CubeEntity() override;

    void think(std::chrono::duration<double, std::milli> delta);

    void setEntityPosition(const glm::vec3 &vec);
};


#endif //STAGE_FIGHTER_CUBEENTITY_H
