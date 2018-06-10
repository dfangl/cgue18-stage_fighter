//
// Created by raphael on 19.03.18.
//

#include <spdlog/spdlog.h>
#include "ModelManager.h"

#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

std::string ModelManager::root;
std::map<std::string, std::shared_ptr<tinygltf::Model>> ModelManager::models;
tinygltf::TinyGLTF ModelManager::loader;
std::shared_ptr<spdlog::logger> ModelManager::logger = spdlog::stdout_color_mt(std::string("Gltf Loader"));

void ModelManager::build(const std::string &root) {
    ModelManager::root = root;
}

void ModelManager::destroy() {
    ModelManager::models.clear();
}

std::shared_ptr<tinygltf::Model> ModelManager::load(const std::string &name) {
    if(models.find(name) == models.end()) {
        logger->info("Loading {}.gltf model file", name);

        std::string error;
        auto model = new tinygltf::Model();
        auto ret = loader.LoadASCIIFromFile(model, &error, (root + name + ".gltf"));

        if(!error.empty())
            ModelManager::logger->error(error);

        if(!ret) {
            ModelManager::logger->error("Unable to load file: {}", name);
            ModelManager::logger->flush();

            // If mingw is used on Windows, save the file with LF endings and try again ...
            throw std::runtime_error(std::string("Unable to load model file: ") + root + name + ".gltf" + "!");
        }

        models[name] = std::shared_ptr<tinygltf::Model>(model);
    }

    return models[name];
}

void ModelManager::store(const std::string &name, const std::shared_ptr<tinygltf::Model> &model) {
    models[name] = model;
}
