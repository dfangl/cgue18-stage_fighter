//
// Created by Raphael on 13.06.2018.
//

#include "ScriptedObject.h"


#include "../helper/CompilerMacros.h"
#include "../level/LuaClassWrapper.h"
#include "../Scene.h"

ScriptedObject::ScriptedObject(const glm::vec3 &pos, const glm::quat &rot, btCollisionShape* bulletShape, btScalar mass,
                               float bsRadius, const std::shared_ptr<tinygltf::Model> &model, const std::vector<std::shared_ptr<Shader>> &shaders,
                               int instances, kaguya::LuaTable env, const std::shared_ptr<BulletUniverse> &world)
        : AbstractScriptedObject(env, false),
          BulletObject(btVector3(pos.x, pos.y, pos.z), btQuaternion(rot.y, rot.z, rot.w, rot.w), bulletShape, mass),
          Model3DObject(position, bsRadius, model, shaders, instances), world(world) {
    Model3DObject::setRotation(rot);
    this->updateModelMatrix();

    this->position = pos;

    environment["setLinearVelocity"] = kaguya::function([this](LuaVec3 &vel) { this->rigidBody->setLinearVelocity(vel.toVector3()); });
    environment["setGravity"] = kaguya::function([this](LuaVec3 &g) { this->rigidBody->setGravity(g.toVector3()); });
    environment["setAngularFactor"] = kaguya::function([this](LuaVec3 &vel) { this->rigidBody->setAngularFactor(vel.toVector3()); });
    environment["getPosition"] = kaguya::function([this]() -> LuaVec3 { return this->position; } );
    environment["spawnParticleSystem"] = kaguya::function([this](LuaScriptedParticleSystem &ps, LuaVec3 &pos, int particles) {
        auto p = ps.toParticleSystem(this->position + pos.vec3, particles);
        this->particleSystem.push_back(p);
        this->offsets.push_back(pos.vec3);
        p->init();
    });
    environment["setRotation"] = kaguya::function([this](LuaVec4 &rot) {
        dynamic_cast<BulletObject*>(this)->setRotation(rot.toQuat());
        dynamic_cast<Model3DObject*>(this)->setRotation(rot.toGlmQuat());
    });
    environment["disableDeactivation"] = kaguya::function([this](){ rigidBody->setActivationState(DISABLE_DEACTIVATION); });

    luaInit();

    this->stickyToPlayer = getOrDefault("sticky", false);
    this->kind = getOrDefault("kind", BulletObject::ENVIRONMENT);

    world->addRigidBody(BulletObject::rigidBody);
}

void ScriptedObject::think(Level* UNUSED(level), std::chrono::duration<double, std::milli> delta) {
    auto bT = BulletObject::getTransformation();
    auto o = bT.getOrigin();
    auto r = bT.getRotation();

    this->position = glm::vec3(o.x(), o.y(), o.z());
    Model3DObject::setRotation(glm::quat(r.w(), r.x(), r.y(), r.z()));
    Model3DObject::setOrigin(position);

    for (size_t i = 0; i < particleSystem.size(); i++) {
        particleSystem[i]->setOrigin(position + offsets[i]);
    }

    luaThink(delta);
}

ScriptedObject::~ScriptedObject() {
    world->removeRigidBody(BulletObject::rigidBody);
}

void ScriptedObject::update(Scene *scene) {
    for (auto &p : particleSystem) p->update(scene);
}

void ScriptedObject::show(Scene *scene) {
    for (auto &p : particleSystem) scene->addParticleSystem(p);
}

void ScriptedObject::hide(Scene *scene) {
    for (auto &p : particleSystem) scene->removeParticleSystem(p);
}
