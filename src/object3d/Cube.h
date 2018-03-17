//
// Created by raphael on 15.03.18.
//

#ifndef STAGE_FIGHTER_CUBE_H
#define STAGE_FIGHTER_CUBE_H


#include "Object3D.h"
#include "../Texture.h"

class Cube: public Object3D {

private:
    static const float vertices[];
    std::shared_ptr<Texture> texture;

public:
    Cube(const glm::vec3 &pos, const std::shared_ptr<Texture> &texture);
    virtual ~Cube();

    void draw() override;
};


#endif //STAGE_FIGHTER_CUBE_H
