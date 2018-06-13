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
#include "../object3d/particlesystem/ScriptedParticleSystem.h"

#include "../helper/CompilerMacros.h"
#include "../manager/TextureManager.h"

class LuaScriptedParticleSystem; //#include "../level/LuaClassWrapper.h"

#define INST_PREALLOC (300)
#define PARTILCE_COUNT (4)

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

        std::shared_ptr<ScriptedParticleSystem> smoke;

        Projectile(const btVector3 &pos, const btVector3 &target, InstancedProjectile *parent, float speed);

        void collideWith(BulletObject* other) override;
        void move(std::chrono::duration<double, std::milli> delta);
    };

    std::shared_ptr<BulletUniverse> world;
    const LuaScriptedParticleSystem *ps;
    //std::shared_ptr<InstancedParticleSystem> smoke;

    std::vector<std::shared_ptr<Projectile>> projectiles;

    btVector3 collisionBox;
    btScalar mass;
    float bsRadius;

    glm::vec3 fakePos = glm::vec3(0,0,0);
    int renderPass = 0;

public:
    InstancedProjectile(float bsRadius, const std::shared_ptr<tinygltf::Model> &model, const std::shared_ptr<Shader> &shader,
                        const btVector3 &bulletShape, const btScalar &mass, std::shared_ptr<BulletUniverse> &world,
                        const LuaScriptedParticleSystem *ps);
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
