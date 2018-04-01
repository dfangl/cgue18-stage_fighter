//
// Created by Raphael on 17.03.2018.
//

#ifndef STAGE_FIGHTER_CAMERAENTITY_H
#define STAGE_FIGHTER_CAMERAENTITY_H

#include "../object3d/BulletObject.h"
#include "../entity/Entity.h"
#include "../entity/CubeEntity.h"
#include "../Window.h"

#include <functional>

class CameraEntity : public Entity, public BulletObject {

private:
    Camera &camera;
    btVector3 speed;

    bool forewardPressed = false;
    bool backwardPressed = false;
    bool leftPressed = false;
    bool rightPressed = false;
    bool jump = false;

    std::shared_ptr<BulletUniverse> world;

    std::function<void(Window const *)> keyboardCallback;

    static btVector3 bulletMovementVector;

public:
    explicit CameraEntity(Camera &camera, std::shared_ptr<BulletUniverse> world, btCollisionShape *bulletShape = new btBoxShape(btVector3(0.5,0.5,0.5)), btScalar mass = 1);
    virtual ~CameraEntity();

    void setEntityPosition(const glm::vec3 &vec, const glm::quat &rot) override;
    void lookAt(const glm::vec3 &obj);

    void think(std::chrono::duration<double, std::milli> delta) override;

    virtual void render(const Camera &camera){}

    std::function<void(Window const *)> getKeyboardCallback() const { return this->keyboardCallback; }

    bool enabled = false;
    float entitySpeed = 7.0f;

    glm::vec3 getPosition() {
        return this->camera.getPosition();
    }
};


#endif //STAGE_FIGHTER_CAMERAENTITY_H
