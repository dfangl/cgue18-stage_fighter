//
// Created by raphael on 13.03.18.
//

#include "TextureManager.h"

std::string TextureManager::root;
std::map<std::string, std::shared_ptr<Texture>> TextureManager::textures;

void TextureManager::build(const std::string &root) {
    TextureManager::root = root;
}

void TextureManager::destroy() {
    TextureManager::textures.clear();
}

std::shared_ptr<Texture> TextureManager::load(const std::string &name) {
    if(textures.find(name) == textures.end()) {
        textures[name] = std::make_shared<Texture>(root + name);
    }

    return textures[name];
}