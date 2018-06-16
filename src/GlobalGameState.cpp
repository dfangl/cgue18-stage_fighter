//
// Created by Raphael on 15.06.2018.
//

#include "GlobalGameState.h"

GlobalGameState::State GlobalGameState::state = GlobalGameState::IN_MENU;
std::shared_ptr<Level> GlobalGameState::level = nullptr;