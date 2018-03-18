//
// Created by Raphael on 17.03.2018.
//

#ifndef STAGE_FIGHTER_CAMERAENTITY_H
#define STAGE_FIGHTER_CAMERAENTITY_H

#include "../object3d/BulletObject.h"
#include "Entity.h"
#include "CubeEntity.h"

#include <functional>

class CameraEntity : public BulletObject, public Entity {

private:
    Camera &camera;
    btVector3 speed;

    bool forewardPressed = false;
    bool backwardPressed = false;
    bool leftPressed = false;
    bool rightPressed = false;
    bool jump = false;

    std::shared_ptr<BulletUniverse> world;

    std::function<void(int,int,int,int)> keyboardCallback;

    static btVector3 bulletMovementVector;

public:
    explicit CameraEntity(Camera &camera, std::shared_ptr<BulletUniverse> world, btCollisionShape *bulletShape = new btBoxShape(btVector3(0.5,0.5,0.5)), btScalar mass = 0.1);
    virtual ~CameraEntity();

    void setEntityPosition(const glm::vec3 &vec) override;

    void think(std::chrono::duration<double, std::milli> delta) override;

    std::function<void(int,int,int,int)> getKeyboardCallback() const { return this->keyboardCallback; }

    bool enabled = false;
    float entitySpeed = 3.0f;
};


#endif //STAGE_FIGHTER_CAMERAENTITY_H
