//
// Created by Raphael on 10.06.2018.
//

#ifndef STAGE_FIGHTER_INSTANCEDPROJECTILE_H
#define STAGE_FIGHTER_INSTANCEDPROJECTILE_H

#include <cfloat>
#include <random>

#include "Entity.h"

#include "../BulletUniverse.h"

#include "../object3d/BulletObject.h"
#include "../object3d/Model3DObject.h"
#include "../object3d/ParticleSystem.h"

#include "../helper/CompilerMacros.h"
#include "../manager/TextureManager.h"

#define INST_PREALLOC (300)
#define PARTILCE_COUNT (4)

class InstancedParticleSystem: public ParticleSystem {
public:
    InstancedParticleSystem(const Model3DObject *model)
            : ParticleSystem(glm::vec3(0,0,0), 0.286695f, ShaderManager::load("particlesystem-instanced", true),
                             TextureManager::load("explosion.png"), PARTILCE_COUNT) {
        this->generateParticles(PARTILCE_COUNT);


        glBindVertexArray(VAO);

        const auto mLoc = shader->getAttribLocation("model");
        if (mLoc == -1)
            throw std::runtime_error("Unable to set instanced VAO attributes (sure that is the right shader?)");

        glBindBuffer(GL_ARRAY_BUFFER, model->getInstancedModelVBO());
        shader->setVertexAttributePointer(mLoc + 0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(0 * sizeof(glm::vec4)));
        shader->setVertexAttributePointer(mLoc + 1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));
        shader->setVertexAttributePointer(mLoc + 2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        shader->setVertexAttributePointer(mLoc + 3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
        shader->setVertexAttribDivisor(mLoc, 4, 1);

        glBindVertexArray(0);

    }

protected:
    std::map<unsigned int, size_t> mapping;

    void generateParticles(unsigned int count) override {
        data.reserve(PARTILCE_COUNT * INST_PREALLOC);
    }

public:
    void addParticles(unsigned int instanceID, const glm::vec3 &direction, float speed) {
        mapping[instanceID] = data.size() - 1;

        std::default_random_engine generator;
        std::uniform_int_distribution<int> distribution(550, 825);

        const glm::vec3 spawn = glm::vec3(
                -direction.x * 0.174505f/2,
                -direction.y * 0.174505f/2,
                -direction.z * 0.286695f/2);
        const glm::vec3 vec = -direction * 0.025f * 1.0f/60.0f; // trust me, just a magic number

        for (unsigned int i = 0; i < PARTILCE_COUNT; i++) {
            const float ttl = distribution(generator);

            data.emplace_back(
                    glm::vec4(spawn, ttl - distribution(generator)),
                    glm::vec4(vec, 0.0f),
                    glm::vec4(spawn, ttl)

            );
        }
    }

    void removeParticles(unsigned int instanceID) {
        const auto old = mapping[instanceID];
        const auto end = data.size() - 1;
        unsigned long lastElementID = 0;

        // find lastElementID element:
        for (const auto &inst : mapping) {
            if (inst.second == data.size() - 1) {
                lastElementID = inst.first;
                break;
            }
        }

        mapping[lastElementID] = old;

        for (int i=0; i < PARTILCE_COUNT; i++) {
            spdlog::get("console")->info("{}", old-i);
            data[old-i] = data.back();
            data.pop_back();
        }
    }
};


class InstancedProjectile : /*public Entity,*/ public Model3DObject {

private:
    class Projectile : public BulletObject {
    public:
        int idx;
        unsigned int instanceID;
        InstancedProjectile *parent;
        float lifeTime = 10000.0f; // 10 sec
        float speed;
        glm::vec3 direction;

        Projectile(const btVector3 &pos, const btVector3 &target, InstancedProjectile *parent, float speed);

        void collideWith(BulletObject* other) override;
        void move(std::chrono::duration<double, std::milli> delta);
    };

    std::shared_ptr<BulletUniverse> world;
    std::shared_ptr<InstancedParticleSystem> smoke;

    std::vector<std::shared_ptr<Projectile>> projectiles;

    btVector3 collisionBox;
    btScalar mass;
    float bsRadius;

    glm::vec3 fakePos = glm::vec3(0,0,0);
    bool particleChange = false;

public:
    InstancedProjectile(float bsRadius, const std::shared_ptr<tinygltf::Model> &model, const std::shared_ptr<Shader> &shader,
                        const btVector3 &bulletShape, const btScalar &mass, std::shared_ptr<BulletUniverse> &world);
    ~InstancedProjectile();

    void spawn(const btVector3 &pos, const btVector3 &target, float speed);

    float getBoundingSphereRadius() override { return FLT_MAX; }
    const glm::vec3 &getPosition() const override { return fakePos; }

    void think(std::chrono::duration<double, std::milli> delta);

    void render(Scene *scene);

    std::shared_ptr<ParticleSystem> getParticleSystem() { return this->smoke; };

};


#undef INST_PREALLOC
#undef PARTILCE_COUNT

#endif //STAGE_FIGHTER_INSTANCEDPROJECTILE_H
