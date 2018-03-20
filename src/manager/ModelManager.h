//
// Created by raphael on 19.03.18.
//

#ifndef STAGE_FIGHTER_MODELMANAGER_H
#define STAGE_FIGHTER_MODELMANAGER_H

#include <memory>
#include <spdlog/logger.h>
#include <tiny_gltf.h>

class ModelManager {

private:
    static std::string root;
    static std::map<std::string, std::shared_ptr<tinygltf::Model>> models;
    static tinygltf::TinyGLTF loader;
    static std::shared_ptr<spdlog::logger> logger;

public:
    static void build(const std::string &root = nullptr);
    static void destroy();

    static std::shared_ptr<tinygltf::Model> load(const std::string &name);
    static void store(const std::string &name, const std::shared_ptr<tinygltf::Model> &shader);

};


#endif //STAGE_FIGHTER_MODELMANAGER_H
