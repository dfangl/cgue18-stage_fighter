//
// Created by Raphael on 01.04.2018.
//

#ifndef STAGE_FIGHTER_FONTMANAGER_H
#define STAGE_FIGHTER_FONTMANAGER_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include "../helper/Font.h"

#include <memory>
#include <string>
#include <map>
#include <spdlog/spdlog.h>

class FontManager {
private:

    static FT_Library ft;
    static std::string root;

    static std::map<std::string, std::shared_ptr<Font>> fonts;
    static std::shared_ptr<spdlog::logger> logger;

public:
    static void build(const std::string &root = nullptr);
    static void destroy();

    static std::shared_ptr<Font> load(const std::string &name);
    static void store(const std::string &name, FT_Face &font);
};


#endif //STAGE_FIGHTER_FONTMANAGER_H
