//
// Created by raphael on 11.06.18.
//

#include "ScriptedParticleSystem.h"
#include "../../level/LuaClassWrapper.h"

ScriptedParticleSystem::ScriptedParticleSystem(const glm::vec3 &position, float radius,
                                               const std::shared_ptr<Shader> &shader,
                                               const std::shared_ptr<Texture> &texture, unsigned int particles,
                                               kaguya::LuaTable scriptEnv)
        : ParticleSystem(position, radius, shader, texture, particles) {
    this->scriptEnv = scriptEnv;

    this->scriptEnv["setSize"] = kaguya::function([this](float x, float y) { this->setSize(glm::vec2(x,y)); });
    this->scriptEnv["push_particle"] = kaguya::function([this](LuaVec3 &pos, LuaVec3 &velocity, float cLife, float ttl) {
        this->data.emplace_back(
                    glm::vec4(pos.vec3, cLife),
                    glm::vec4(velocity.vec3, 0.0f),
                    glm::vec4(pos.vec3, ttl)
                );
    });
}


void ScriptedParticleSystem::generateParticles(unsigned int count) {
    data.reserve(count);
    this->scriptEnv["generateParticles"](scriptEnv, count);
    this->loadSSBO();
}

void ScriptedParticleSystem::init() {
    this->scriptEnv["init"](scriptEnv);
    this->particles = getOrDefault("particles", this->particles);
    this->generateParticles(this->particles);
}


