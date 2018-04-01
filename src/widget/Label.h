//
// Created by Raphael on 01.04.2018.
//

#ifndef STAGE_FIGHTER_LABEL_H
#define STAGE_FIGHTER_LABEL_H

#include <memory>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Widget.h"
#include "../helper/Font.h"
#include "../Shader.h"

class Label : public Widget {

private:
    GLuint VAO, VBO;
    glm::vec3 color;
    std::string text;
    float x,y,scale;
    std::shared_ptr<Font> font;

    std::shared_ptr<Shader> shader;

public:
    Label(std::string &text, std::shared_ptr<Font> font, float x, float y, float scale, glm::vec3 color);
    virtual ~Label();

    void render(const glm::mat4 &projection) override;

};


#endif //STAGE_FIGHTER_LABEL_H
