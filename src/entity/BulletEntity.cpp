//
// Created by raphael on 16.04.18.
//

#include "../helper/QuatUtils.h"

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

    // Magic rotation calculation
    glm::quat rotation = Quat::rotateTo(target1 - position);

    Model3DObject::setRotation(rotation);
    Model3DObject::setOrigin(position);
    BulletObject::setOrigin(pos, btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));

    world->addRigidBody(this->rigidBody);
}

void BulletEntity::think(Level *level, std::chrono::duration<double, std::milli> delta) {
    this->maxLifeTime -= delta.count();
    if (maxLifeTime <= 0.0)
        this->health = 0;

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

void BulletEntity::render(Scene *scene) {
    Model3DObject::render(scene);
}
