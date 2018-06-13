//
// Created by Raphael on 13.06.2018.
//

#include "ScriptedObject.h"


#include "../helper/CompilerMacros.h"
#include "../level/LuaClassWrapper.h"

ScriptedObject::ScriptedObject(const glm::vec3 &pos, const glm::quat &rot, btCollisionShape* bulletShape, btScalar mass,
                               float bsRadius, const std::shared_ptr<tinygltf::Model> &model, const std::shared_ptr<Shader> &shader,
                               int instances, kaguya::LuaTable env, const std::shared_ptr<BulletUniverse> &world)
        : AbstractScriptedObject(env, false),
          BulletObject(btVector3(pos.x, pos.y, pos.z), btQuaternion(rot.y, rot.z, rot.w, rot.w), bulletShape, mass),
          Model3DObject(position, bsRadius, model, shader, instances), world(world) {
    this->setRotation(rot);
    this->updateModelMatrix();

    environment["setLinearVelocity"] = kaguya::function([this](LuaVec3 &vel) { this->rigidBody->setLinearVelocity(vel.toVector3()); });
    environment["setGravity"] = kaguya::function([this](LuaVec3 &g) { this->rigidBody->setGravity(g.toVector3()); });
    environment["setAngularVelocity"] = kaguya::function([this](LuaVec3 &vel) { this->rigidBody->setAngularFactor(vel.toVector3()); });
    luaInit();

    world->addRigidBody(BulletObject::rigidBody);
}

void ScriptedObject::think(Level* UNUSED(level), std::chrono::duration<double, std::milli> delta) {
    auto bT = BulletObject::getTransformation();
    auto o = bT.getOrigin();
    auto r = bT.getRotation();

    this->position = glm::vec3(o.x(), o.y(), o.z());
    Model3DObject::setRotation(glm::quat(r.w(), r.x(), r.y(), r.z()));
    Model3DObject::setOrigin(position);

   luaThink(delta);
}

ScriptedObject::~ScriptedObject() {
    world->removeRigidBody(BulletObject::rigidBody);
}
