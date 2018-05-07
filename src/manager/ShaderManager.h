//
// Created by raphael on 13.03.18.
//

#ifndef STAGE_FIGTHER_SHADERMANAGER_H
#define STAGE_FIGTHER_SHADERMANAGER_H

#include <memory>
#include <map>
#include <string>

#include "../Shader.h"

class ShaderManager {

private:
    static std::string root;
    static std::map<std::string, std::shared_ptr<Shader>> shaderLib;

public:
    static void build(const std::string &root = nullptr);
    static void destroy();

    static std::shared_ptr<Shader> load(const std::string &name);
    static void store(const std::string &name, const std::shared_ptr<Shader> &shader);

    static void recompileAll();
};


#endif //STAGE_FIGTHER_SHADERMANAGER_H
