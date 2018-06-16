//
// Created by Raphael on 15.06.2018.
//

#ifndef STAGE_FIGHTER_GLOBALGAMESTATE_H
#define STAGE_FIGHTER_GLOBALGAMESTATE_H

#include <memory>

#include "level/Level.h"

class GlobalGameState {

public:
    enum State {
        IN_MENU, IN_LEVEL, LEVEL_LOADING_FINISHED, JUMP_TO_MAIN_SCREEN, AFTER_LEVEL,
        LEVEL_PLAY_AGAIN
    };

    static State state;
    static std::shared_ptr<Level> level;

};


#endif //STAGE_FIGHTER_GLOBALGAMESTATE_H
