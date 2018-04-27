//
// Created by Raphael on 17.03.2018.
//

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <utility>
#include <memory>

#include "../level/Level.h"

#include "CubeEntity.h"
#include "BulletEntity.h"

CubeEntity::CubeEntity(const glm::vec3 &pos, const std::shared_ptr<Texture> &texture, std::shared_ptr<BulletUniverse> world) :
        Cube(pos, texture),
        BulletObject(btVector3(pos.x,pos.y,pos.z), btQuaternion(0,0,0,1), new btBoxShape(btVector3(0.5,0.5,0.5)), 15.0f) {

    this->world = world;
    this->kind = BulletObject::ENEMY;
    this->name = "Cube";
    this->health = 100;
    this->maxHealth = 100;
    this->mustBeKilled = true;
    this->lastDmgTime = 0.0;

    world->addRigidBody(this->getRigidBody());
}

CubeEntity::~CubeEntity() {
    this->world->removeRigidBody(this->getRigidBody());
}

void CubeEntity::think(std::chrono::duration<double, std::milli> delta) {
    btTransform bT = this->getTransformation();

    auto origin = bT.getOrigin();
    this->position = glm::vec3(origin.x(), origin.y(), origin.z());
    Cube::setOrigin(position);

    auto rotation = bT.getRotation();
    Cube::rotate(glm::quat(
            rotation.w(),
            rotation.x(),
            rotation.y(),
            rotation.z()
    ));

    if (lastDmgTime > 0.0f)
        lastDmgTime -= delta.count();
}

void CubeEntity::setEntityPosition(const glm::vec3 &vec, const glm::quat &rot) {
    this->position = vec;

    Cube::setOrigin(vec);
    BulletObject::setOrigin(btVector3(vec.x,vec.y,vec.z));

    // Reactivate even if Bullet removed all forces
    this->getRigidBody()->activate(true);
}

void CubeEntity::render(const Camera &camera, const std::vector<std::shared_ptr<Light>> &lights) {
    Cube::render(camera, lights);
}

void CubeEntity::collideWith(BulletObject *other) {
    if (other->getKind() == BulletObject::ENVIRONMENT)
        return;

    if (lastDmgTime > 0.0f)
        return;

    // TODO: remove Player
    if (other->getKind() == BulletObject::PLAYER || other->getKind() == BulletObject::WEAPON) {
        lastDmgTime = dmgTimeout;
        this->health -= 1;
    }
}

void CubeEntity::think(Level *level, std::chrono::duration<double, std::milli> delta) {
    think(delta);

    if (this->health == 0)
        return;

    lastSpawnTime -= delta.count();
    if (lastSpawnTime < 0.0f) {
        lastSpawnTime = spawnSpeed;

        auto origin = getTransformation().getOrigin();
        auto pPos = level->getPlayer()->getEntityPosition();

        level->spawn(std::make_shared<BulletEntity>(origin, btVector3(pPos.x, pPos.y, pPos.z), world));
    }

}
