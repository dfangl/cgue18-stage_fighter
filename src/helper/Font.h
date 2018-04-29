//
// Created by Raphael on 01.04.2018.
//

#ifndef STAGE_FIGHTER_FONT_H
#define STAGE_FIGHTER_FONT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <memory>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Logger.h"

class Font : Logger {

public:
    struct Character {
        glm::ivec2 size;       // Size of glyph
        glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
        long int   advance;    // Offset to advance to next glyph
        glm::vec2  texCoord;   // Size in the Texture Atlas
        float      tx;         // Texture x offset
    };

    explicit Font(FT_Face &face, unsigned int size=12);
    ~Font();

    void preload(std::string chars);

    std::shared_ptr<Font> setSize(unsigned int size);
    Character &get(unsigned long c);

    int getSize() { return s; }
    size_t getAtlasWidth() { return texAtlasSize; }
    GLuint getAtlasTexID() { return textureAtlasID; }

    void renderTextureAtlas();

private:
    int s;
    FT_Face face;

    std::map<unsigned long, Character> characters;
    std::vector<unsigned long> texData;

    size_t texAtlasSize = 0;
    GLuint textureAtlasID = 0;
};


#endif //STAGE_FIGHTER_FONT_H
