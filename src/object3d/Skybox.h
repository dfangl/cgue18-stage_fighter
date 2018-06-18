//
// Created by raphael on 14.05.18.
//

#ifndef STAGE_FIGHTER_SKYBOX_H
#define STAGE_FIGHTER_SKYBOX_H


#include "Object3D.h"

#include "../CubemapTexture.h"

class Skybox : public Object3D {

private:
    static const float vertices[];

    const std::shared_ptr<CubemapTexture> cubemap;

public:
    Skybox(const std::shared_ptr<CubemapTexture> &cubemap, const std::vector<std::shared_ptr<Shader>> &shader);

    void render(Scene *scene) override;

    void draw(std::shared_ptr<Shader> &shader) override;
};


#endif //STAGE_FIGHTER_SKYBOX_H
