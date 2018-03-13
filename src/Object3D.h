//
// Created by raphael on 13.03.18.
//

#ifndef STAGE_FIGTHER_OBJECT3D_H
#define STAGE_FIGTHER_OBJECT3D_H

#include <glad/glad.h>

class Object3D {

protected:
    GLuint VBO, VAO, EBO;

public:
    virtual void draw() = 0;

};


#endif //STAGE_FIGTHER_OBJECT3D_H
