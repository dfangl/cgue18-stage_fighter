//
// Created by Raphael on 01.04.2018.
//

#ifndef STAGE_FIGHTER_FONT_H
#define STAGE_FIGHTER_FONT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H
#include "Logger.h"

class Font : Logger {

public:
    struct Character {
        GLuint     textureID;  // ID handle of the glyph texture
        glm::ivec2 size;       // Size of glyph
        glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
        long int   advance;    // Offset to advance to next glyph
    };

    explicit Font(FT_Face &face, unsigned int size=12);
    ~Font();
/*
    void preload(std::string chars);
    void preload(int start, int end);*/

    void setSize(unsigned int size);
    Character &get(unsigned long c);

private:
    FT_Face face;
    std::map<unsigned long, Character> characters;

};


#endif //STAGE_FIGHTER_FONT_H
