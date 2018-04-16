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
    btVector3 speed;
    float height = 0.9f * 2.0f; // Capsule height * 2 [radius]
    const float airTime = 500.0f; // in ms

    bool forewardPressed = false;
    bool backwardPressed = false;
    bool leftPressed = false;
    bool rightPressed = false;

    float jump = 0.0f;
    bool canJump = false;
    bool enabled = true;

    std::shared_ptr<BulletUniverse> world;
    std::function<void(Window const *)> keyboardCallback;

    static btVector3 bulletMovementVector;

protected:
    Camera &camera;

public:
    explicit CameraEntity(Camera &camera,
                          std::shared_ptr<BulletUniverse> world,
                          btCollisionShape *bulletShape = new btSphereShape(0.7f),
                          btScalar mass = 1.0f);

    virtual ~CameraEntity();

    float entitySpeed = 7.0f;
    float jumpSpeed = 14.0f;

    void enable();
    void disable();

    void setEntityPosition(const glm::vec3 &vec, const glm::quat &rot) override;
    glm::vec3 getEntityPosition() const override { return this->camera.getPosition(); }
    void lookAt(const glm::vec3 &obj);

    void think(std::chrono::duration<double, std::milli> delta) override;

    std::function<void(Window const *)> getKeyboardCallback() const { return this->keyboardCallback; }

    void collideWith(BulletObject *other) override;


    void render(const Camera &camera) override {}

    glm::vec3 isInView(const Entity *entity);

};


#endif //STAGE_FIGHTER_CAMERAENTITY_H
