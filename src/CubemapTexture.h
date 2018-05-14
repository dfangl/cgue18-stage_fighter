//
// Created by raphael on 14.05.18.
//

#ifndef STAGE_FIGHTER_CUBEMAPTEXTURE_H
#define STAGE_FIGHTER_CUBEMAPTEXTURE_H

#include <string>

#include "Texture.h"

#define CUBEMAP_FACES (6)

class CubemapTexture : public Texture {

private:
    static const std::string locationNames[CUBEMAP_FACES];

public:
    CubemapTexture(const std::string &path, const std::string &extension);

    void bind(GLenum texUnit) override;


};


#endif //STAGE_FIGHTER_CUBEMAPTEXTURE_H
