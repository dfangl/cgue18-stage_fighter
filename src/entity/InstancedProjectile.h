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
#include "BulletEntity.h"

#define INST_PREALLOC (300)
#define PARTILCE_COUNT (4)

/*
class InstancedParticleSystem: public ParticleSystem {
public:
    InstancedParticleSystem(const Model3DObject *model)
            : ParticleSystem(glm::vec3(0,0,0), 0.286695f, ShaderManager::load("particlesystem-instanced", true),
                             TextureManager::load("explosion.png"), PARTILCE_COUNT) {
        this->generateParticles(PARTILCE_COUNT);


        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        const auto mLoc = shader->getAttribLocation("model");
        if (mLoc == -1)
            throw std::runtime_error("Unable to set instanced VAO attributes (sure that is the right shader?)");

        shader->setVertexAttribDivisor("position", 1);
        shader->setVertexAttribDivisor("maxTTL", 1);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        shader->setVertexAttributePointer(mLoc + 0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(0 * sizeof(glm::vec4)));
        shader->setVertexAttributePointer(mLoc + 1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));
        shader->setVertexAttributePointer(mLoc + 2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        shader->setVertexAttributePointer(mLoc + 3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
        shader->setVertexAttribDivisor(mLoc, 4, 1);

        glBindVertexArray(0);

    }

protected:
    std::map<void *, size_t> mapping;
    std::vector<glm::mat4> positions;

    void generateParticles(unsigned int count) override {
        data.reserve(PARTILCE_COUNT * INST_PREALLOC);
    }

public:
    void addParticles(void *key, const glm::vec3 &position, const glm::vec3 &direction, float speed) {
        mapping[key] = ((mapping.size() + 1) * PARTILCE_COUNT) - 1;

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
            positions.push_back(glm::translate(glm::mat4(1.0f), position));
        }
    }

    void removeParticles(void *key) {
        const auto old = mapping[key];
        void *lastElementID = nullptr;

        // find lastElementID element:
        for (const auto &inst : mapping) {
            if (inst.second == data.size() - 1) {
                lastElementID = inst.first;
                break;
            }
        }

        mapping[lastElementID] = old;
        mapping.erase(key);

        for (int i = 0; i < PARTILCE_COUNT; i++) {
            data[old - i] = data.back();
            data.pop_back();
        }

        const auto pIDX = ((old + 1) / PARTILCE_COUNT) - 1;
        positions[pIDX] = positions.back();
        positions.pop_back();
    }

    void updateEmitterPosition(void *key, const glm::vec3 &position) {
        const auto old = mapping[key];
        const auto pIDX = ((old + 1) / PARTILCE_COUNT) - 1;
        positions[pIDX] = glm::translate(glm::mat4(1.0f), position);
    }

    void update() {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::mat4), positions.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        this->uploadSSBOtoGPU();
    }

    void draw() override {
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);

        texture->bind(GL_TEXTURE0);
        shader->setUniform("particle_size", this->particle_size);

        glBindVertexArray(VAO);
        glDrawArraysInstanced(GL_POINTS, 0, particles, positions.size());
        glBindVertexArray(0);

        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
    }
};
*/

class InstancedProjectile : /*public Entity,*/ public Model3DObject, virtual public AbstractParticleSystem {

private:
    class Projectile : public BulletObject {
    public:
        int idx;
        unsigned int instanceID;
        InstancedProjectile *parent;
        float lifeTime = 10000.0f; // 10 sec
        float speed;
        glm::vec3 direction;

        std::shared_ptr<BulletEntitySmokeParticleSystem> smoke;

        Projectile(const btVector3 &pos, const btVector3 &target, InstancedProjectile *parent, float speed);

        void collideWith(BulletObject* other) override;
        void move(std::chrono::duration<double, std::milli> delta);
    };

    std::shared_ptr<BulletUniverse> world;
    //std::shared_ptr<InstancedParticleSystem> smoke;

    std::vector<std::shared_ptr<Projectile>> projectiles;

    btVector3 collisionBox;
    btScalar mass;
    float bsRadius;

    glm::vec3 fakePos = glm::vec3(0,0,0);
    int renderPass = 0;

public:
    InstancedProjectile(float bsRadius, const std::shared_ptr<tinygltf::Model> &model, const std::shared_ptr<Shader> &shader,
                        const btVector3 &bulletShape, const btScalar &mass, std::shared_ptr<BulletUniverse> &world);
    ~InstancedProjectile();

    void spawn(const btVector3 &pos, const btVector3 &target, float speed);

    float getBoundingSphereRadius() override { return FLT_MAX; }
    const glm::vec3 &getPosition() const override { return fakePos; }

    void think(std::chrono::duration<double, std::milli> delta);

    void render(Scene *scene) override;
    void update(Scene *scene) override;

protected:
    void generateParticles(unsigned int count) override;


};


#undef INST_PREALLOC
#undef PARTILCE_COUNT

#endif //STAGE_FIGHTER_INSTANCEDPROJECTILE_H
