//
// Created by Raphael on 01.04.2018.
//

#ifndef STAGE_FIGHTER_FONTMANAGER_H
#define STAGE_FIGHTER_FONTMANAGER_H

#include <memory>
#include <string>
#include <map>

#include <spdlog/spdlog.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../helper/Font.h"

class FontManager {
private:

    static FT_Library ft;
    static std::string root;

    static std::map<std::string, std::shared_ptr<Font>> fonts;
    static std::shared_ptr<spdlog::logger> logger;

public:
    static void build(const std::string &root = nullptr);
    static void destroy();

    static std::shared_ptr<Font> load(const std::string &file, const std::string &name, int size);
    static void store(const std::string &name, std::shared_ptr<Font> &font);
    static std::shared_ptr<Font> get(const std::string &name);
};


#endif //STAGE_FIGHTER_FONTMANAGER_H
