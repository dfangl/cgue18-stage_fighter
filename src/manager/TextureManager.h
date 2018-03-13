//
// Created by raphael on 13.03.18.
//

#ifndef STAGE_FIGTHER_TEXTUREMANAGER_H
#define STAGE_FIGTHER_TEXTUREMANAGER_H

#include <map>
#include <memory>
#include "../Texture.h"

class TextureManager {
private:
    static std::string root;
    static std::map<std::string, std::shared_ptr<Texture>> textures;

public:
    static void build(const std::string &root = nullptr);
    static void destroy();

    static std::shared_ptr<Texture> load(const std::string &name);
};


#endif //STAGE_FIGTHER_TEXTUREMANAGER_H
