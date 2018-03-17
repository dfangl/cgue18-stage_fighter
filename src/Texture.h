//
// Created by raphael on 13.03.18.
//

#ifndef STAGE_FIGTHER_TEXTURE_H
#define STAGE_FIGTHER_TEXTURE_H

#include <glad/glad.h>
#include <string>
#include "helper/Logger.h"

class Texture : Logger {

private:
    GLuint textureID;

public:
    explicit Texture(const std::string &path);
    ~Texture();

    void bind(GLenum texUnit);

};


#endif //STAGE_FIGTHER_TEXTURE_H
