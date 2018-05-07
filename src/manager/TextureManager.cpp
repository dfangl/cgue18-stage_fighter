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

std::shared_ptr<Texture> TextureManager::load(const tinygltf::Image &image, const tinygltf::Sampler &sampler) {
    if(textures.find(image.name) == textures.end()) {
        textures[image.name] = std::make_shared<Texture>(image, sampler);
    }

    return textures[image.name];
}

void TextureManager::store(const std::string &name, std::vector<unsigned char> &data, int width, int height, bool rgba) {
    textures[name] = std::make_shared<Texture>(data, height, width, rgba);
}
