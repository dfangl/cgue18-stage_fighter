//
// Created by raphael on 16.04.18.
//

#ifndef STAGE_FIGHTER_BULLETENTITY_H
#define STAGE_FIGHTER_BULLETENTITY_H


#include <random>

#include "../BulletUniverse.h"
#include "../object3d/BulletObject.h"
#include "../object3d/Model3DObject.h"

#include "Entity.h"

#include "../manager/TextureManager.h"

#define BULLET_PARTILCE_COUNT (8)

class BulletEntitySmokeParticleSystem: public ParticleSystem {
public:
    BulletEntitySmokeParticleSystem(const glm::vec3 &position, const glm::vec3 &direction, const float &speed) : ParticleSystem(
            position, 0.286695f, ShaderManager::load("particlesystem", true), TextureManager::load("explosion.png"), BULLET_PARTILCE_COUNT),
                                                                                                                 direction(direction), speed(speed) {
        this->generateParticles(BULLET_PARTILCE_COUNT);
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

class BulletEntity : public Entity, public BulletObject, public Model3DObject {

private:
    std::shared_ptr<BulletUniverse> world;
    glm::vec3 direction;

    float speed = 5.0f;
    float maxLifeTime = 10000.0f; // 10 sec

    std::shared_ptr<ParticleSystem> smoke;

public:
    BulletEntity(const btVector3 &pos, const btVector3 &target, std::shared_ptr<BulletUniverse> &world);
    ~BulletEntity();

    void think(Level *level, std::chrono::duration<double, std::milli> delta) override;

    void collideWith(BulletObject *other) override;

    void setPosition(const glm::vec3 &vec, const glm::quat &rot) override;

    void think(std::chrono::duration<double, std::milli> delta) override;

    void render(Scene *scene) override;

    float getBoundingSphereRadius() override;

    const glm::vec3 &getPosition() const override;

    const std::shared_ptr<ParticleSystem> &getSmoke() const { return this->smoke; }

    Kind getEntityKind() override;
};


#endif //STAGE_FIGHTER_BULLETENTITY_H
