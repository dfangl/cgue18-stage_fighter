//
// Created by raphael on 07.05.18.
//

#ifndef STAGE_FIGHTER_TEXTUREGENERATOR_H
#define STAGE_FIGHTER_TEXTUREGENERATOR_H

#include <vector>

#include <glm/glm.hpp>

class ImageGenerator {

public:
    static std::vector<unsigned char> marble(unsigned int width, unsigned int height, glm::vec4 color, bool rgba = false);

};


#endif //STAGE_FIGHTER_TEXTUREGENERATOR_H
