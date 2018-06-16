//
// Created by Raphael on 01.04.2018.
//

#include <algorithm>

#include "Font.h"

Font::Font(FT_Face &_face, unsigned int size) : s(size) {
    this->face = _face;
    FT_Set_Pixel_Sizes(face, 0, size);
    glGenTextures(1, &this->textureAtlasID);
}

Font::~Font() {
    glDeleteTextures(1, &this->textureAtlasID);
    FT_Done_Face(face);
}

Font::Character &Font::get(unsigned long c) {
    auto elem = this->characters.find(c);

    if(elem == characters.end()) {
        logger->info("Load glyph: {}", c);

        this->texData.push_back(c);
        renderTextureAtlas();

        return characters[c];
    }

    return (*elem).second;
}

std::shared_ptr<Font> Font::setSize(unsigned int size) {
    return std::make_shared<Font>(this->face, size);
}

void Font::renderTextureAtlas() {
    // Calculate Texture size:
    unsigned int texHeight = 0;
    unsigned int texWidth  = 0;
    for (auto &glyph : this->texData) {
        if(FT_Load_Char(face, glyph, FT_LOAD_RENDER)) {
            logger->error("Unable to load Glyph {} for font!", glyph);
            throw std::runtime_error("Unable to load Glyph for font!");
        }

        texHeight = std::max(texHeight, face->glyph->bitmap.rows);
        texWidth += face->glyph->bitmap.width;
    }

    texAtlasSize = texWidth;

    glBindTexture(GL_TEXTURE_2D, this->textureAtlasID);
    // Disable 4 byte restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            texWidth,
            texHeight,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            nullptr
    );

    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Copy all the glyphs to texture
    GLint offset = 0;
    characters.clear();
    for (auto &glyph : this->texData) {
        FT_Load_Char(face, glyph, FT_LOAD_RENDER);

        if (face->glyph->bitmap.buffer != nullptr)
            glTexSubImage2D(
                    GL_TEXTURE_2D,
                    0,
                    offset,
                    0,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer
            );
        else {
            logger->error("Glyph '{}' (ASCII-Code:{}) does not have a font texture!", (char)glyph, glyph);
        }

        characters[glyph] = {
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x,
                glm::vec2(
                        (float)face->glyph->bitmap.width / (float)texWidth,
                        (float)face->glyph->bitmap.rows / (float)texHeight
                ),
                (float)offset / (float)texWidth
        };

        offset += face->glyph->bitmap.width;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Font::preload(std::string chars) {
    for (auto &c : chars) {
        if (this->characters.find(static_cast<const unsigned long &>(c)) == this->characters.end())
            this->texData.push_back(static_cast<unsigned long &&>(c));
    }

    renderTextureAtlas();
}
