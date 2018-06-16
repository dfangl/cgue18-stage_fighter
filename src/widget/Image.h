//
// Created by Raphael on 15.06.2018.
//

#ifndef STAGE_FIGHTER_IMAGE_H
#define STAGE_FIGHTER_IMAGE_H

#include <memory>
#include <vector>

#include "Widget.h"
#include "../Texture.h"
#include "../Shader.h"

class Image : public Widget {

private:
    GLuint VAO = 0, VBO = 0;
    std::shared_ptr<Texture> imgTexture;
    std::vector<float> glBuffer;
    std::shared_ptr<Shader> shader;

    float x, y;

public:
    Image(float x, float y, float width, float height, const std::shared_ptr<Texture> &image);
    Image(Image&) = delete;
    Image &operator=(Image&) = delete;
    ~Image();

    void setImage(const std::shared_ptr<Texture> &image);

    void render(const glm::mat4 &projection, float screenGamma) override;
    void resize(float x, float y) override;

};


#endif //STAGE_FIGHTER_IMAGE_H
