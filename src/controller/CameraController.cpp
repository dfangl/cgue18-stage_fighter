//
// Created by Raphael on 17.03.2018.
//

#include "CameraController.h"
#include "../manager/TextureManager.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/quaternion.hpp>


btVector3 CameraEntity::bulletMovementVector(0, 1, 0);

CameraEntity::CameraEntity(Camera &camera, std::shared_ptr<BulletUniverse> world, btCollisionShape *bulletShape, btScalar mass) :
        BulletObject(btVector3(camera.getPosition().x,camera.getPosition().y,camera.getPosition().z), btQuaternion(0,0,0,1), bulletShape, mass), camera(camera){
    this->speed = btVector3(0.0, 0.0, 0.0);
    this->world = world;
    this->keyboardCallback = [this](Window const *window) {
        this->forewardPressed  = window->getKey(GLFW_KEY_W) != GLFW_RELEASE;
        this->leftPressed      = window->getKey(GLFW_KEY_A) != GLFW_RELEASE;
        this->backwardPressed  = window->getKey(GLFW_KEY_S) != GLFW_RELEASE;
        this->rightPressed     = window->getKey(GLFW_KEY_D) != GLFW_RELEASE;

        if (canJump && jump <= 0 && window->getKey(GLFW_KEY_SPACE) != GLFW_RELEASE) {
            rigidBody->applyCentralImpulse(btVector3(0,this->jumpSpeed,0));
            jump = airTime;
        }
    };

    rigidBody->setActivationState(DISABLE_DEACTIVATION);
    rigidBody->setFriction(1.0f);
    rigidBody->setRestitution(0.1f);
    this->kind = BulletObject::PLAYER;

    //rigidBody->setDamping(0, 200);
    world->addRigidBody(rigidBody);
}

void CameraEntity::setEntityPosition(const glm::vec3 &vec, const glm::quat &rot) {
    this->camera.update(vec);
    BulletObject::setOrigin(btVector3(vec.x,vec.y,vec.z), btQuaternion(rot.x, rot.y, rot.z, rot.w));
}

void CameraEntity::think(std::chrono::duration<double, std::milli> delta) {
    if (!enabled)
        return;

    auto bT = this->getTransformation();
    auto o = bT.getOrigin();
    this->camera.update(glm::vec3(o.x(), o.y(), o.z()));

    float zVelocity = 0.0f;
    float xVelocity = 0.0f;

    if (this->forewardPressed) xVelocity =  this->entitySpeed;
    if (this->backwardPressed) xVelocity = -this->entitySpeed;
    if (this->leftPressed)     zVelocity = -this->entitySpeed;
    if (this->rightPressed)    zVelocity =  this->entitySpeed;

    speed.setZ( canJump ? zVelocity : zVelocity/2 );
    speed.setX( canJump ? xVelocity : xVelocity/2 );
    speed.setY(0.0f);

    auto x = rigidBody->getLinearVelocity();
    speed.setY(x.y());

    rigidBody->setLinearVelocity(speed.rotate(bulletMovementVector, btRadians(-camera.getYaw())));

    {
        auto end = btVector3(o.x(), o.y()-height, o.z());
        //auto end2 = btVector3(o.x(), o.y()-height/2, o.z());
        auto &start = o;
        btCollisionWorld::ClosestRayResultCallback rayCallback(start, end);
        //btCollisionWorld::ClosestRayResultCallback glitchingCallback(start, end2);
        world->rayTest(start, end, rayCallback);
        //world->rayTest(start, end2, glitchingCallback);

        canJump = rayCallback.hasHit();
        if (canJump && jump > 0) {
            jump -= delta.count();
        }

        /*
        if (glitchingCallback.hasHit()) {
           speed.setY(0.00001f);
           BulletObject::setOrigin(btVector3(o.x(), o.y()+height/2+0.0001f, o.z()));
        }
         */
    }


    //rigidBody->setAngularFactor(btVector3(0,1,0));
    //rigidBody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
}

CameraEntity::~CameraEntity() {
    world->removeRigidBody(BulletObject::rigidBody);
}

void CameraEntity::lookAt(const glm::vec3 &obj) {
    this->camera.lookAt(obj);
}

void CameraEntity::collideWith(BulletObject *other) {
    if (other->getKind() == BulletObject::ENVIRONMENT)
        return;

    spdlog::get("console")->info("Collided with: {}", other->getKind());
}

void CameraEntity::enable() {
    this->enabled = true;
    this->camera.enableUpdate = true;
}

void CameraEntity::disable() {
    this->enabled = false;
    this->camera.enableUpdate = false;
}

glm::vec3 CameraEntity::isInView(const Entity *object) {
    return camera.project(object->getEntityPosition());
}
