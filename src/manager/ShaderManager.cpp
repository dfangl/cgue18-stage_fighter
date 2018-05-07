//
// Created by raphael on 13.03.18.
//

#include "ShaderManager.h"

std::string ShaderManager::root;
std::map<std::string, std::shared_ptr<Shader>> ShaderManager::shaderLib;

void ShaderManager::build(const std::string &root) {
    ShaderManager::root = root;
}

void ShaderManager::destroy() {
    ShaderManager::shaderLib.clear();
}

std::shared_ptr<Shader> ShaderManager::load(const std::string &name) {
    if(shaderLib.find(name) == shaderLib.end()) {
        shaderLib[name] = Shader::fromFile(root + name + ".vert", root + name + ".frag");
    }

    return shaderLib[name];
}

void ShaderManager::store(const std::string &name, const std::shared_ptr<Shader> &shader) {
    shaderLib[name] = shader;
}

void ShaderManager::recompileAll() {
    for (auto &shader : shaderLib)
        shader.second->recompile();
}
