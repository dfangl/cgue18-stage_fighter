//
// Created by Raphael on 01.04.2018.
//

#ifndef STAGE_FIGHTER_WIDGET_H
#define STAGE_FIGHTER_WIDGET_H

#include <glm/glm.hpp>

class Widget {

public:
    virtual void render(const glm::mat4 &projection, float screenGamma = 1.0) = 0;
    virtual void resize(float x, float y) = 0;

};

#endif //STAGE_FIGHTER_WIDGET_H
