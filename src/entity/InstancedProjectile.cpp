//
// Created by Raphael on 10.06.2018.
//

#include <random>

#include "InstancedProjectile.h"
#include "../helper/QuatUtils.h"
#include "../manager/TextureManager.h"
#include "../level/LuaClassWrapper.h"

#define INST_PREALLOC (300)


InstancedProjectile::Projectile::Projectile(const btVector3 &pos, const btVector3 &target, InstancedProjectile *parent, float speed)
        : parent(parent), BulletObject(pos, btQuaternion(0,0,0,1),  new btBoxShape(parent->collisionBox), parent->mass) {
    this->speed = speed;
    this->kind = BulletObject::BULLET;

    const glm::vec3 position = glm::vec3(pos.x(), pos.y(), pos.z());
    const glm::vec3 target1 = glm::vec3(target.x(), target.y(), target.z());
    this->direction = glm::normalize(target1 - position);

    // Magic rotation calculation
    glm::quat rotation = Quat::rotateTo(target1 - position);
    BulletObject::setOrigin(pos, btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));

    this->instanceID = parent->addInstance(glm::vec3(pos.x(), pos.y(), pos.z()), rotation);
    this->smoke = parent->ps->toParticleSystem(position, 8);
    this->smoke->setValue("direction", LuaVec3(direction));
    this->smoke->setValue("hitbox", LuaVec3(parent->collisionBox.x(),parent->collisionBox.y(),parent->collisionBox.z()));
    this->smoke->init();

//    parent->smoke->addParticles(this, glm::vec3(vec3.x(), vec3.y(), vec3.z()), direction, speed);
}

void InstancedProjectile::Projectile::move(std::chrono::duration<double, std::milli> delta) {
    if (this->idx < 0)
        return; // projectile is not alive?

    // Let Projectile die after a while
    lifeTime -= delta.count();
    if (lifeTime <= 0) {
        this->idx = -idx;
        return;
    }

    const glm::vec3 vec = direction * speed;

    auto bT = BulletObject::getTransformation();
    auto o = bT.getOrigin();
    auto r = bT.getRotation();
    const glm::vec3 position = glm::vec3(o.x(), o.y(), o.z());

    parent->setInstance(instanceID, position, glm::quat(r.w(), r.x(), r.y(), r.z()));

    BulletObject::rigidBody->setLinearVelocity(btVector3(vec.x, vec.y, vec.z));
    BulletObject::rigidBody->setGravity(btVector3(0,0,0));

    //parent->smoke->updateEmitterPosition(this, position);
    this->smoke->setOrigin(position);
}

void InstancedProjectile::Projectile::collideWith(BulletObject* UNUSED(other)) {
    if (this->idx >= 0) {
        this->idx = -idx;
    }
}

InstancedProjectile::InstancedProjectile(float bsRadius, const std::shared_ptr<tinygltf::Model> &model,
                                         const std::shared_ptr<Shader> &shader, const btVector3 &bulletShape,
                                         const btScalar &mass, std::shared_ptr<BulletUniverse> &world,
                                         const LuaScriptedParticleSystem *ps)
        : Model3DObject(glm::vec3(0,0,0), bsRadius, model, shader, INST_PREALLOC),
          collisionBox(bulletShape), mass(mass), world(world), ps(ps) {
    this->projectiles.reserve(INST_PREALLOC);
    this->clearInstances();
    //this->smoke = std::make_shared<InstancedParticleSystem>(this);
}

InstancedProjectile::~InstancedProjectile() {
    for (auto &p : this->projectiles) {
        world->removeRigidBody(p->getRigidBody());
    }
}

void InstancedProjectile::think(std::chrono::duration<double, std::milli> delta) {
    this->projectiles.erase(
            std::remove_if(
                    this->projectiles.begin(),
                    this->projectiles.end(),
                    [this](std::shared_ptr<Projectile> current) -> bool {
                        if (current->idx < 0) {
                            this->world->removeRigidBody(current->getRigidBody());
                            this->removeInstance(current->instanceID);
                            //smoke->removeParticles(this);
                            return true;
                        }

                        return false;
                    }
            ),
            this->projectiles.end()
    );

    for (auto &p : this->projectiles)
        p->move(delta);
}

void InstancedProjectile::render(Scene *scene) {
    if (renderPass++ == 0) Model3DObject::render(scene);
    else {
        for (auto &pps : projectiles) pps->smoke->render(scene);
        //smoke->render(scene);
        renderPass = 0;
    }
}

void InstancedProjectile::spawn(const btVector3 &pos, const btVector3 &target, float speed) {
    const auto projectile = std::make_shared<Projectile>(pos, target, this, speed);
    projectile->idx = static_cast<int>(projectiles.size() - 1);

    projectiles.push_back(projectile);
    world->addRigidBody(projectile->getRigidBody());
}

void InstancedProjectile::update(Scene *scene) {
    for (auto &pps : projectiles)
        pps->smoke->update(scene);
    //smoke->update();
}

void InstancedProjectile::generateParticles(unsigned int UNUSED(count)) {}
