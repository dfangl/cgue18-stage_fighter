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
    float height = 0.9f * 2.0f; // Capsule height * 2 [radius]

    bool forewardPressed = false;
    bool backwardPressed = false;
    bool leftPressed = false;
    bool rightPressed = false;
    bool jump = false;
    bool canJump = false;

    std::shared_ptr<BulletUniverse> world;
    std::function<void(Window const *)> keyboardCallback;

    static btVector3 bulletMovementVector;

public:
    explicit CameraEntity(Camera &camera,
                          std::shared_ptr<BulletUniverse> world,
                          btCollisionShape *bulletShape = new btCapsuleShape(0.5f, 0.9f),
                          btScalar mass = 1.0f);

    virtual ~CameraEntity();


    bool enabled = false;
    float entitySpeed = 7.0f;

    void setEntityPosition(const glm::vec3 &vec, const glm::quat &rot) override;
    void lookAt(const glm::vec3 &obj);

    void think(std::chrono::duration<double, std::milli> delta) override;

    std::function<void(Window const *)> getKeyboardCallback() const { return this->keyboardCallback; }

    void collideWith(BulletObject *other) override;


    virtual void render(const Camera &camera){}

    glm::vec3 getPosition() const {
        return this->camera.getPosition();
    }
};


#endif //STAGE_FIGHTER_CAMERAENTITY_H
