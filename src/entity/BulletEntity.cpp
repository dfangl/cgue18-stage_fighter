//
// Created by raphael on 16.04.18.
//

#include <glm/gtc/quaternion.hpp>
#include "../manager/ModelManager.h"
#include "../manager/ShaderManager.h"

#include "BulletEntity.h"

BulletEntity::BulletEntity(const btVector3 &pos, const btVector3 &target, std::shared_ptr<BulletUniverse> &world) :
        BulletObject(pos, btQuaternion(0,0,0,1), new btBoxShape(btVector3(0.242388f/2.0f,0.242388f/2.0f,0.242388f/2.0f)), 0.00001),
        Model3DObject(ModelManager::load("bullet"), ShaderManager::load("cube")) {

    this->health = 1;
    this->maxHealth = 0;
    this->world = world;

    const glm::vec3 position = glm::vec3(pos.x(), pos.y(), pos.z());
    const glm::vec3 target1 = glm::vec3(target.x(), target.y(), target.z());
    this->direction = glm::normalize(position - target1);

    spdlog::get("console")->info("direction: {},{},{}", position.x, position.y, position.z);

    Model3DObject::setOrigin(position);
    world->addRigidBody(this->rigidBody);
}

void BulletEntity::think(Level *level, std::chrono::duration<double, std::milli> delta) {
    if (this->health == 0)
        return;

    //const auto newPos = position; //+ direction * speed * (float)delta.count();

    //spdlog::get("console")->info("Position: {},{},{}", newPos.x, newPos.y, newPos.z);

    //Model3DObject::setOrigin(newPos);
    //BulletObject::setOrigin(btVector3(newPos.x, -35.0f, newPos.z), btQuaternion(0,0,0,1));
}

void BulletEntity::collideWith(BulletObject *other) {

    // Todo: only ignore own spawn enemy object
    if (other->getKind() == BulletObject::ENEMY)
        return;

    this->health = 0;
    spdlog::get("console")->info("ded");
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

void BulletEntity::render(const Camera &camera) {
    Model3DObject::render(camera);
}
