//
// Created by raphael on 16.04.18.
//

#include <utility>
#include <random>

#include "../helper/QuatUtils.h"

#include "../manager/ModelManager.h"
#include "../manager/ShaderManager.h"

#include "../object3d/ParticleSystem.h"

#include "../level/Level.h"

#include "BulletEntity.h"

#define FRUSTUM_CULLING_RADIUS (0.286695f/2)
#define BULLET_COLLISION_BOX btVector3(0.174505f/2,0.174505f/2,0.286695f/2)
#define PARTILCE_COUNT (4)

class BulletEntitySmokeParticleSystem: public ParticleSystem {
public:
    BulletEntitySmokeParticleSystem(const glm::vec3 &position, const glm::vec3 &direction, const float &speed) : ParticleSystem(
            position, 0.286695f, ShaderManager::load("particlesystem", true), TextureManager::load("explosion.png"), PARTILCE_COUNT),
            direction(direction), speed(speed) {
        this->generateParticles(PARTILCE_COUNT);
    }

protected:
    const glm::vec3 &direction;
    const float &speed;

    void generateParticles(unsigned int count) override {
        data.reserve(count);

        std::default_random_engine generator;
        std::uniform_int_distribution<int> distribution(550, 825);

        const glm::vec3 spawn = glm::vec3(
                -direction.x * 0.174505f/2,
                -direction.y * 0.174505f/2,
                -direction.z * 0.286695f/2);
        const glm::vec3 vec = -direction * 0.025f * 1.0f/60.0f; // trust me, just a magic number

        for (unsigned int i = 0; i < count; i++) {
            const float ttl = distribution(generator);

            data.emplace_back(
                    glm::vec4(spawn, ttl - distribution(generator)),
                    glm::vec4(vec, 0.0f),
                    glm::vec4(spawn, ttl)

            );
        }

        ParticleSystem::loadSSBO();
    }
};

BulletEntity::BulletEntity(const btVector3 &pos, const btVector3 &target, std::shared_ptr<BulletUniverse> &world) :
        BulletObject(pos, btQuaternion(0,0,0,1), new btBoxShape(BULLET_COLLISION_BOX), 0.00001),
        Model3DObject(glm::vec3(pos.x(), pos.y(), pos.z()), FRUSTUM_CULLING_RADIUS, ModelManager::load("bullet"), ShaderManager::load("standard")) {

    this->health = 1;
    this->maxHealth = 0;
    this->world = world;
    this->kind = BulletObject::BULLET;

    const glm::vec3 position = glm::vec3(pos.x(), pos.y(), pos.z());
    const glm::vec3 target1 = glm::vec3(target.x(), target.y(), target.z());
    this->direction = glm::normalize(target1 - position);

    this->smoke = std::make_shared<BulletEntitySmokeParticleSystem>(
            glm::vec3(pos.x(), pos.y(), pos.z()),
            this->direction,
            this->speed
    );

    this->smoke->setSize(glm::vec2(0.2f, 0.2f));

    // Magic rotation calculation
    glm::quat rotation = Quat::rotateTo(target1 - position);

    Model3DObject::setRotation(rotation);
    Model3DObject::setOrigin(position);
    BulletObject::setOrigin(pos, btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));

    world->addRigidBody(this->rigidBody);
}

void BulletEntity::think(Level *level, std::chrono::duration<double, std::milli> delta) {
    this->maxLifeTime -= delta.count();
    if (maxLifeTime <= 0.0)
        this->health = 0;

    if (this->health == 0) {
        level->despawn(this);
        return;
    }

    const glm::vec3 vec = direction * speed;

    auto bT = BulletObject::getTransformation();
    auto o = bT.getOrigin();
    auto r = bT.getRotation();

    Model3DObject::setRotation(glm::quat(r.w(), r.x(), r.y(), r.z()));
    Model3DObject::setOrigin(glm::vec3(o.x(), o.y(), o.z()));

    BulletObject::rigidBody->setLinearVelocity(btVector3(vec.x, vec.y, vec.z));
    BulletObject::rigidBody->setGravity(btVector3(0,0,0));

    smoke->setOrigin(glm::vec3(o.x(), o.y(), o.z()));
}

void BulletEntity::collideWith(BulletObject *other) {
    // Todo: only ignore own spawn enemy object
    if (other->getKind() == BulletObject::ENEMY)
        return;

    this->health = 0;
}

BulletEntity::~BulletEntity() {
    world->removeRigidBody(this->rigidBody);
}

void BulletEntity::setPosition(const glm::vec3 &vec, const glm::quat &rot) {
    Model3DObject::setOrigin(vec);
    BulletObject::setOrigin(btVector3(vec.x, vec.y, vec.z), btQuaternion(rot.x, rot.y, rot.z, rot.w));
}

// Never called
void BulletEntity::think(std::chrono::duration<double, std::milli> UNUSED(delta)) {}

void BulletEntity::render(Scene *scene) {
    Model3DObject::render(scene);
}

float BulletEntity::getBoundingSphereRadius() {
    return Object3D::boundingSphereRadius;
}

const glm::vec3 &BulletEntity::getPosition() const {
    return this->position;
}

BulletObject::Kind BulletEntity::getEntityKind() {
    return BulletObject::BULLET;
}
