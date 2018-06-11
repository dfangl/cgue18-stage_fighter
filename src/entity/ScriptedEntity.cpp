//
// Created by raphael on 24.05.18.
//

#include <glm/glm.hpp>

#include "ScriptedEntity.h"

#include "../manager/ModelManager.h"

#include "../level/Level.h"
#include "../level/LuaClassWrapper.h"

#include "../helper/CompilerMacros.h"

ScriptedEntity::ScriptedEntity(const std::string &name, int health, const btVector3 &pos, float bsRadius, const btQuaternion &rot,
                               std::string model, float mass, glm::vec3 collisionOffset, btCollisionShape *hitbox,
                               const std::shared_ptr<BulletUniverse> &world, kaguya::LuaTable scriptEnv)
        : BulletObject(pos, rot, hitbox, mass),
          Model3DObject(glm::vec3(pos.x(),pos.y(),pos.z()), bsRadius, ModelManager::load(model), ShaderManager::load("standard")) {

    this->world = world;
    this->environment = scriptEnv;
    this->collisionOffset = collisionOffset;

    this->health = health;
    this->maxHealth = health;
    this->name = name;

    this->mustBeKilled = getOrDefault("mustBeKilled", true);
    //TODO: Parse more values from script environment
    this->kind = BulletObject::ENEMY;
    this->dmgTimeout = 250.0f;

    this->environment["getSpawnPosFor"] = kaguya::function([this](LuaVec3 &pPos) -> LuaVec3 {
        auto origin = this->getTransformation().getOrigin();

        btVector3 center;
        float radius;
        BulletObject::fallShape->getBoundingSphere(center, radius);
        glm::vec3 direction = glm::normalize(pPos.vec3 - position) * radius;

        return LuaVec3(origin.x() + direction.x, origin.y() + direction.y, origin.z() + direction.z);
    });

    this->environment["getHealth"] = kaguya::function([this]() -> int { return this->getHealth(); });
    this->environment["getPosition"] = kaguya::function([this]() -> LuaVec3 { return LuaVec3(this->position); });


    this->position = glm::vec3(pos.x(), pos.y(), pos.z());
    Model3DObject::setRotation(glm::quat(rot.w(), rot.x(), rot.y(), rot.z()));
    Model3DObject::setOrigin(position + collisionOffset);

    world->addRigidBody(BulletObject::rigidBody);
}

ScriptedEntity::~ScriptedEntity() {
    world->removeRigidBody(BulletObject::rigidBody);
}

void ScriptedEntity::think(Level *level, std::chrono::duration<double, std::milli> delta) {
    if (this->health <= 0) {
        level->despawn(this);
        return;
    }

    auto bT = BulletObject::getTransformation();
    auto o = bT.getOrigin();
    auto r = bT.getRotation();

    this->position = glm::vec3(o.x(), o.y(), o.z());
    Model3DObject::setRotation(glm::quat(r.w(), r.x(), r.y(), r.z()));
    Model3DObject::setOrigin(position + collisionOffset);

    this->scriptTimeout += delta.count();
    if (this->scriptTimeout >= SCRIPT_THINK_TIMEOUT) {
        this->environment["think"].call<void>(environment, delta.count());
        this->scriptTimeout = 0;
    }

    //if (lastDmgTime > 0.0)
    lastDmgTime -= delta.count();
}

void ScriptedEntity::setPosition(const glm::vec3 &vec, const glm::quat &rot) {
    this->position = vec;
    Model3DObject::setOrigin(vec + collisionOffset);
    BulletObject::setOrigin(btVector3(vec.x, vec.y, vec.z), btQuaternion(rot.x, rot.y, rot.z, rot.w));
}

void ScriptedEntity::collideWith(BulletObject* UNUSED(other)) {}
void ScriptedEntity::think(std::chrono::duration<double, std::milli> UNUSED(delta)) {}

void ScriptedEntity::render(Scene *scene) {
    Model3DObject::render(scene);
}

float ScriptedEntity::getBoundingSphereRadius() {
    return Object3D::boundingSphereRadius;
}

const glm::vec3 &ScriptedEntity::getPosition() const {
    return this->position;
}

BulletObject::Kind ScriptedEntity::getEntityKind() {
    return BulletObject::getKind();
}
