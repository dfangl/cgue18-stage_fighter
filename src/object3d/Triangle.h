//
// Created by raphael on 13.03.18.
//

#ifndef STAGE_FIGTHER_TRIANGLE_H
#define STAGE_FIGTHER_TRIANGLE_H

#include <glm/glm.hpp>
#include "Object3D.h"
#include "../Shader.h"
#include "../Texture.h"
#include <memory>

class Triangle: public Object3D {

private:
    static const float vertices[];
    std::shared_ptr<Texture> texture;

public:
    explicit Triangle(const glm::vec3 &pos, const std::shared_ptr<Texture> &texture);
    ~Triangle();

    void draw() override;

};


#endif //STAGE_FIGTHER_TRIANGLE_H
