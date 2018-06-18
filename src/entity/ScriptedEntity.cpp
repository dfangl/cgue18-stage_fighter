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
                               const std::string &model, float mass, glm::vec3 collisionOffset, btCollisionShape *hitbox,
                               const std::shared_ptr<BulletUniverse> &world, kaguya::LuaTable scriptEnv)
        : AbstractScriptedObject(scriptEnv, false), BulletObject(pos, rot, hitbox, mass),
          Model3DObject(glm::vec3(pos.x(),pos.y(),pos.z()), bsRadius, ModelManager::load(model), std::vector<std::shared_ptr<Shader>> {ShaderManager::load("standard")}) {

    this->world = world;
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

    environment["getHealth"] = kaguya::function([this]() -> int { return this->getHealth(); });
    environment["getPosition"] = kaguya::function([this]() -> LuaVec3 { return LuaVec3(this->position); });
    environment["setLinearVelocity"] = kaguya::function([this](LuaVec3 &vel) { this->rigidBody->setLinearVelocity(vel.toVector3()); });
    environment["setGravity"] = kaguya::function([this](LuaVec3 &g) { this->rigidBody->setGravity(g.toVector3()); });
    environment["setAngularVelocity"] = kaguya::function([this](LuaVec3 &vel) { this->rigidBody->setAngularFactor(vel.toVector3()); });

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

    luaThink(delta);

    //if (lastDmgTime > 0.0)
    lastDmgTime -= delta.count();
}

void ScriptedEntity::setPosition(const glm::vec3 &vec, const glm::quat &rot) {
    this->position = vec + collisionOffset;
    Model3DObject::setOrigin(position);
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

