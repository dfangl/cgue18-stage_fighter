//
// Created by Raphael on 13.06.2018.
//

#ifndef STAGE_FIGHTER_ABSTRACTSCRIPTEDOBJECT_H
#define STAGE_FIGHTER_ABSTRACTSCRIPTEDOBJECT_H

#include <utility>

#include <kaguya/lua_ref.hpp>

#define SCRIPT_THINK_TIMEOUT (1.0/120.0)

class AbstractScriptedObject {

protected:
    double scriptTimeout;
    kaguya::LuaTable environment;

    template<typename T>
    inline T getOrDefault(const std::string &property, T defaultValue) {
        if (environment[property].isNilref())   return defaultValue;
        else                                    return environment[property].get<T>();
    }

    void luaThink(std::chrono::duration<double, std::milli> delta) {
        this->scriptTimeout += delta.count();
        if (this->scriptTimeout >= SCRIPT_THINK_TIMEOUT) {
            this->environment["think"].call<void>(environment, delta.count());
            this->scriptTimeout = 0;
        }
    }

    void luaInit() {
        this->environment["init"].call<void>(environment);
    }

public:
    AbstractScriptedObject(kaguya::LuaTable env, bool doInit = false) : scriptTimeout(0),environment(env) {
        if(doInit) luaInit();
    }

};


#endif //STAGE_FIGHTER_ABSTRACTSCRIPTEDOBJECT_H
