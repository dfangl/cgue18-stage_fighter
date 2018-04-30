//
// Created by raphael on 26.04.18.
//


#include "../level/Level.h"
#include "../manager/ModelManager.h"

#include "EnemyEntity.h"
#include "BulletEntity.h"

EnemyEntity::EnemyEntity(std::string name, int health, int spawnTime, const btVector3 &pos, const btQuaternion &rot, std::string model,
                         float mass, glm::vec3 collisionOffset, btCollisionShape* hitbox, const std::shared_ptr<BulletUniverse> &world) :
        BulletObject(pos, rot, hitbox, mass),
        Model3DObject(ModelManager::load(model), ShaderManager::load("standard")){

    this->health = health;
    this->maxHealth = health;
    this->dmgTimeout = 250.0f;
    this->name = name;
    this->mustBeKilled = true;
    this->kind = BulletObject::ENEMY;
    this->collisionOffset = collisionOffset;

    this->world = world;
    this->spawnSpeed = spawnTime;
    this->lastSpawnTime = this->spawnSpeed * 2;

    this->position = glm::vec3(pos.x(), pos.y(), pos.z()) + collisionOffset;
    Model3DObject::setRotation(glm::quat(rot.w(), rot.x(), rot.y(), rot.z()));
    Model3DObject::setOrigin(position);

    world->addRigidBody(BulletObject::rigidBody);
}

EnemyEntity::~EnemyEntity() {
    world->removeRigidBody(BulletObject::rigidBody);
}

void EnemyEntity::think(Level *level, std::chrono::duration<double, std::milli> delta) {
    if (this->health <= 0) {
        level->despawn(this);
        return;
    }

    auto bT = BulletObject::getTransformation();
    auto o = bT.getOrigin();
    auto r = bT.getRotation();

    this->position = glm::vec3(o.x(), o.y(), o.z()) + collisionOffset;
    Model3DObject::setRotation(glm::quat(r.w(), r.x(), r.y(), r.z()));
    Model3DObject::setOrigin(position);

    lastSpawnTime -= delta.count();
    if (lastSpawnTime < 0.0f) {
        lastSpawnTime = spawnSpeed;

        auto origin = getTransformation().getOrigin();
        auto pPos = level->getPlayer()->getEntityPosition();

        // Spawn Projectile outside of HitBox:
        {
            btVector3 center;
            float radius;
            BulletObject::fallShape->getBoundingSphere(center, radius);
            glm::vec3 direction = glm::normalize(pPos - position);

            level->spawn(
                    std::make_shared<BulletEntity>(
                            btVector3(origin.x() + direction.x * radius, origin.y() + direction.y * radius, origin.z() + direction.z * radius),
                            btVector3(pPos.x, pPos.y + 0.35f, pPos.z),
                            world
                    )
            );
        }
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

    /*
    if (other->getKind() == BulletObject::WEAPON || other->getKind() == BulletObject::PLAYER) {
        spdlog::get("console")->info("{} got hit by {}", name, (void*)other);
        auto *player = (Player*)other;
        auto &t = player->getTransformation().getOrigin();
        spdlog::get("console")->info("Player Transform: {},{},{}", t.x(), t.y(), t.z());
        spdlog::get("console")->info("Player Position : {},{},{}", player->getEntityPosition().x, player->getEntityPosition().y, player->getEntityPosition().z);

        t = this->getTransformation().getOrigin();
        spdlog::get("console")->info("this Transform  : {},{},{}", t.x(), t.y(), t.z());
        spdlog::get("console")->info("this Position   : {},{},{}", this->position.x, this->position.y, this->position.z);

        lastDmgTime = dmgTimeout;
        this->health -= 1;
    }
    */
}

void EnemyEntity::think(std::chrono::duration<double, std::milli> delta) {}

void EnemyEntity::render(Scene *scene) {
    Model3DObject::render(scene);
}

void EnemyEntity::receiveDamage(int points) {
    this->health -= points;
}
