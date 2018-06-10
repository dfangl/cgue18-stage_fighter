//
// Created by Raphael on 10.06.2018.
//

#include "InstancedProjectile.h"
#include "../helper/QuatUtils.h"

#define INST_PREALLOC (300)

InstancedProjectile::Projectile::Projectile(const btVector3 &pos, const btVector3 &target, InstancedProjectile *parent, float speed)
        : parent(parent), BulletObject(pos, btQuaternion(0,0,0,1),  new btBoxShape(parent->collisionBox), parent->mass) {
    this->speed = speed;

    const glm::vec3 position = glm::vec3(pos.x(), pos.y(), pos.z());
    const glm::vec3 target1 = glm::vec3(target.x(), target.y(), target.z());
    this->direction = glm::normalize(target1 - position);

    // Magic rotation calculation
    glm::quat rotation = Quat::rotateTo(target1 - position);
    BulletObject::setOrigin(pos, btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));

    //TODO: First frame is kinda broken?
}

void InstancedProjectile::Projectile::move(std::chrono::duration<double, std::milli> delta) {
    const glm::vec3 vec = direction * speed;

    auto bT = BulletObject::getTransformation();
    auto o = bT.getOrigin();
    auto r = bT.getRotation();

    parent->setInstance(instanceID, glm::vec3(o.x(), o.y(), o.z()), glm::quat(r.w(), r.x(), r.y(), r.z()));

    BulletObject::rigidBody->setLinearVelocity(btVector3(vec.x, vec.y, vec.z));
    BulletObject::rigidBody->setGravity(btVector3(0,0,0));
}

void InstancedProjectile::Projectile::collideWith(BulletObject* UNUSED(other)) {
    if (this->idx >= 0) {
        parent->deads.push_back(this->idx);
        this->idx = -1;
    }
}

InstancedProjectile::InstancedProjectile(float bsRadius, const std::shared_ptr<tinygltf::Model> &model,
                                         const std::shared_ptr<Shader> &shader, const btVector3 &bulletShape,
                                         const btScalar &mass, std::shared_ptr<BulletUniverse> &world)
        : Model3DObject(glm::vec3(0,0,0), bsRadius, model, shader, INST_PREALLOC),
          collisionBox(bulletShape), mass(mass), world(world) {
    this->projectiles.reserve(INST_PREALLOC);
    this->clearInstances();
}

InstancedProjectile::~InstancedProjectile() {
    for (auto p : this->projectiles) {
        world->removeRigidBody(p->getRigidBody());
        delete p;
    }
}

void InstancedProjectile::think(std::chrono::duration<double, std::milli> delta) {
    for (auto dead : this->deads)
        removeIDX(dead);

    for (auto p : this->projectiles)
        p->move(delta);
}

void InstancedProjectile::render(Scene *scene) {
    Model3DObject::render(scene);
}

void InstancedProjectile::spawn(const btVector3 &pos, const btVector3 &target, float speed) {
    const auto projectile = new Projectile(pos, target, this, speed);

    projectiles.push_back(projectile);
    world->addRigidBody(projectile->getRigidBody());

    projectile->instanceID = this->addInstance(glm::vec3(pos.x(), pos.y(), pos.z()), glm::quat(1, 0, 0 ,0));
    projectile->idx = static_cast<int>(projectiles.size() - 1);
}

void InstancedProjectile::removeIDX(int idx) {
    const auto old = projectiles[idx];
    const auto end = projectiles.size() - 1;

    projectiles[idx] = projectiles[end];
    projectiles[idx]->idx = idx;
    projectiles.pop_back();

    this->removeInstance(old->instanceID);

    world->removeRigidBody(old->getRigidBody());
    delete old;
}
