//
// Created by Raphael on 01.04.2018.
//

#include "FontManager.h"

FT_Library FontManager::ft;
std::string FontManager::root;

std::map<std::string, std::shared_ptr<Font>> FontManager::fonts;
std::shared_ptr<spdlog::logger> FontManager::logger = spdlog::stdout_color_mt(std::string("Font Manager"));

void FontManager::build(const std::string &root) {
    if (FT_Init_FreeType(&ft)) {
        logger->error("Unable to initialize the Freetype library!");
        logger->flush();

        throw std::runtime_error("Unable to initialize the Freetype library!");
    }

    FontManager::root = root;
}

void FontManager::destroy() {
    fonts.clear();
    FT_Done_FreeType(ft);
}

std::shared_ptr<Font> FontManager::load(const std::string &name) {
    if(fonts.find(name) == fonts.end()) {
        FT_Face face;
        if (FT_New_Face(ft, (root + name + ".ttf").c_str(), 0, &face)){
            logger->error("Unable to load font file: {}.ttf",root + name);
            logger->flush();

            throw std::runtime_error("Unable to load font file: " + root + name + ".ttf");
        }

        fonts[name] = std::make_shared<Font>(face);
    }

    return fonts[name];
}

void FontManager::store(const std::string &name, FT_Face &font) {
    fonts[name] = std::make_shared<Font>(font);
}
