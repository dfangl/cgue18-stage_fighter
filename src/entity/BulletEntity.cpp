//
// Created by raphael on 16.04.18.
//

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>

#include "../manager/ModelManager.h"
#include "../manager/ShaderManager.h"

#include "../level/Level.h"

#include "BulletEntity.h"

BulletEntity::BulletEntity(const btVector3 &pos, const btVector3 &target, std::shared_ptr<BulletUniverse> &world) :
        BulletObject(pos, btQuaternion(0,0,0,1), new btBoxShape(btVector3(0.174505f/2,0.174505f/2,0.286695f/2)), 0.00001),
        Model3DObject(ModelManager::load("bullet"), ShaderManager::load("standard")) {

    this->health = 1;
    this->maxHealth = 0;
    this->world = world;
    this->kind = BulletObject::BULLET;

    const glm::vec3 position = glm::vec3(pos.x(), pos.y(), pos.z());
    const glm::vec3 target1 = glm::vec3(target.x(), target.y(), target.z());
    this->direction = glm::normalize(target1 - position);

    // Magic rotation calculation (does somehow not work as expected ...)
    glm::quat rotation;
    {
        const glm::vec3 end   = glm::normalize(position);
        const glm::vec3 start = glm::normalize(target1);


        const float cosTheta = glm::dot(start, end);
        glm::vec3 rotationAxis;

        // special case when vectors in opposite directions, there is no "ideal" rotation axis
        // So guess one; any will do as long as it's perpendicular to start
        if (cosTheta < -1 + 0.001f){
            rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);

            // they were parallel, try again!
            if (glm::length2(rotationAxis) < 0.01 )
                rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

            rotationAxis = normalize(rotationAxis);
            rotation = glm::angleAxis(glm::radians(180.0f), rotationAxis);
        } else {
            rotationAxis = glm::cross(start, end);

            float s = sqrt( (1+cosTheta)*2 );
            float invs = 1 / s;

            rotation = glm::quat(
                    s * 0.5f,
                    rotationAxis.x * invs,
                    rotationAxis.y * invs,
                    rotationAxis.z * invs
            );
        }
    }


    Model3DObject::setRotation(rotation);
    Model3DObject::setOrigin(position);
    BulletObject::setOrigin(pos, btQuaternion(rotation.x, rotation.y, rotation.z, rotation.z));

    world->addRigidBody(this->rigidBody);
}

void BulletEntity::think(Level *level, std::chrono::duration<double, std::milli> delta) {
    if (this->health == 0) {
        level->despawn(this);
        return;
    }

    const glm::vec3 vec = direction * speed;

    auto bT = BulletObject::getTransformation();
    auto o = bT.getOrigin();
    auto r = bT.getRotation();

    Model3DObject::setRotation(glm::quat(r.w(), r.x(), r.y(), r.z()));
    Model3DObject::setOrigin(glm::vec3(o.x(), o.y(), o.z()));

    BulletObject::rigidBody->setLinearVelocity(btVector3(vec.x, vec.y, vec.z));
}

void BulletEntity::collideWith(BulletObject *other) {
    // Todo: only ignore own spawn enemy object
    if (other->getKind() == BulletObject::ENEMY)
        return;

    this->health = 0;
}

BulletEntity::~BulletEntity() {
    world->removeRigidBody(this->rigidBody);
}

void BulletEntity::setEntityPosition(const glm::vec3 &vec, const glm::quat &rot) {
    Model3DObject::setOrigin(vec);
    BulletObject::setOrigin(btVector3(vec.x, vec.y, vec.z), btQuaternion(rot.x, rot.y, rot.z, rot.w));
}

// Never called
void BulletEntity::think(std::chrono::duration<double, std::milli> delta) {}

void BulletEntity::render(const Camera &camera, const std::vector<std::shared_ptr<Light>> &lights) {
    Model3DObject::render(camera, lights);
}
