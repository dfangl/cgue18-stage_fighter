//
// Created by raphael on 20.06.18.
//

#ifndef STAGE_FIGHTER_AUDIOMANAGER_H
#define STAGE_FIGHTER_AUDIOMANAGER_H

#include <map>
#include <irrKlang.h>

#include "../level/LuaClassWrapper.h"

class AudioManager {
public:
    static irrklang::ISoundEngine *audioEngine;
};


class LuaSound {
private:
    irrklang::ISound *sound;


public:
    explicit LuaSound(irrklang::ISound *s) : sound(s) {
        spdlog::get("console")->info("LuaSound({})", (void*)s);

        if (s == nullptr)
            throw std::runtime_error("Unable to load Sound!");
    }

    void stop() { sound->stop(); }
    void pause() { sound->setIsPaused(true); }
    void resume() { sound->setIsPaused(false); }
    void loop(bool l) { sound->setIsLooped(l); }
    void setPosition(const LuaVec3 &position) {
        sound->setPosition(irrklang::vec3df(static_cast<irrklang::ik_f32>(position.x()),
                                            static_cast<irrklang::ik_f32>(position.y()),
                                            static_cast<irrklang::ik_f32>(position.z())));
    }
    void destroy() { sound->drop(); }
    void setVolume(float volume) { sound->setVolume(volume); }
};


#endif //STAGE_FIGHTER_AUDIOMANAGER_H
