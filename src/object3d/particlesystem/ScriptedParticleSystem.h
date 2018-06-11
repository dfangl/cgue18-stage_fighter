//
// Created by raphael on 11.06.18.
//

#ifndef STAGE_FIGHTER_SCRIPTEDPARTICLESYSTEM_H
#define STAGE_FIGHTER_SCRIPTEDPARTICLESYSTEM_H

#include <kaguya/kaguya.hpp>
#include "../ParticleSystem.h"

class ScriptedParticleSystem : public ParticleSystem {

private:
    kaguya::LuaTable scriptEnv;

protected:
    void generateParticles(unsigned int count) override;

public:
    ScriptedParticleSystem(const glm::vec3 &position, float radius, const std::shared_ptr<Shader> &shader,
                           const std::shared_ptr<Texture> &texture, unsigned int particles, kaguya::LuaTable scriptEnv);

    void init();

    template <typename T>
    void setValue(const std::string &name, T value) {
        this->scriptEnv[name] = value;
    }

    template<typename T>
    inline T getOrDefault(const std::string &property, T defaultValue) {
        if (scriptEnv[property].isNilref())   return defaultValue;
        else                                  return scriptEnv[property].get<T>();
    }

};


#endif //STAGE_FIGHTER_SCRIPTEDPARTICLESYSTEM_H
