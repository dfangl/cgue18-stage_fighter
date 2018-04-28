//
// Created by raphael on 26.04.18.
//


#include "../level/Level.h"
#include "../manager/ModelManager.h"

#include "EnemyEntity.h"
#include "BulletEntity.h"

EnemyEntity::EnemyEntity(std::string name, int health, int spawnTime, const btVector3 &pos, const btQuaternion &rot, std::string model,
                         float mass, btCollisionShape* hitbox, const std::shared_ptr<BulletUniverse> &world) :
        BulletObject(pos, rot, hitbox, mass),
        Model3DObject(ModelManager::load(model), ShaderManager::load("standard")){

    this->health = health;
    this->maxHealth = health;
    this->dmgTimeout = 250.0f;
    this->name = name;
    this->mustBeKilled = true;
    this->kind = BulletObject::ENEMY;

    this->world = world;
    this->spawnSpeed = spawnTime;
    this->lastSpawnTime = this->spawnSpeed * 2;

    this->position = glm::vec3(pos.x(), pos.y(), pos.z());
    Model3DObject::setRotation(glm::quat(rot.w(), rot.x(), rot.y(), rot.z()));
    Model3DObject::setOrigin(glm::vec3(pos.x(), pos.y(), pos.z()));

    world->addRigidBody(BulletObject::rigidBody);
}

EnemyEntity::~EnemyEntity() {
    world->removeRigidBody(BulletObject::rigidBody);
}

void EnemyEntity::think(Level *level, std::chrono::duration<double, std::milli> delta) {
    if (this->health == 0) {
        level->despawn(this);
        return;
    }

    auto bT = BulletObject::getTransformation();
    auto o = bT.getOrigin();
    auto r = bT.getRotation();

    this->position = glm::vec3(o.x(), o.y(), o.z());
    Model3DObject::setRotation(glm::quat(r.w(), r.x(), r.y(), r.z()));
    Model3DObject::setOrigin(glm::vec3(o.x(), o.y(), o.z()));

    lastSpawnTime -= delta.count();
    if (lastSpawnTime < 0.0f) {
        lastSpawnTime = spawnSpeed;

        auto origin = getTransformation().getOrigin();
        auto pPos = level->getPlayer()->getEntityPosition();

        level->spawn(std::make_shared<BulletEntity>(origin, btVector3(pPos.x, pPos.y, pPos.z), world));
    }

    if (lastDmgTime > 0.0)
        lastDmgTime -= delta.count();
}

void EnemyEntity::setEntityPosition(const glm::vec3 &vec, const glm::quat &rot) {
    this->position = vec;
    Model3DObject::setOrigin(vec);
    BulletObject::setOrigin(btVector3(vec.x, vec.y, vec.z), btQuaternion(rot.x, rot.y, rot.z, rot.w));
}

void EnemyEntity::collideWith(BulletObject *other) {
    // Health loss calculation:
    if (lastDmgTime > 0.0)
        return;

    if (other->getKind() == BulletObject::WEAPON || other->getKind() == BulletObject::PLAYER) {
        lastDmgTime = dmgTimeout;
        this->health -= 1;
    }
}

void EnemyEntity::think(std::chrono::duration<double, std::milli> delta) {}

void EnemyEntity::render(Scene *scene) {
    Model3DObject::render(scene);
}
