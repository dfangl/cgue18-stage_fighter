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
    GLuint VAO = 0, VBO = 0;
    glm::vec3 color;
    std::string text;
    float x,y,scale,w;
    std::shared_ptr<Font> font;
    std::shared_ptr<Shader> shader;

    struct Quad {
        GLfloat vertices[6][4];
    };
    std::vector<Quad> vboData;
    void buildVBO();

public:
    Label(std::string text, std::shared_ptr<Font> font, float x, float y, float scale, glm::vec3 color);
    virtual ~Label();

    // This Class contains OpenGL resources,
    // so disable copy const. and = operator
    Label(const Label&) = delete;
    Label &operator=(const Label &) = delete;

    void render(const glm::mat4 &projection) override;
    void resize(float x, float y) override;

    float getWidth();
    void setPosition(float x, float y);
    void setText(const std::string &text);

};


#endif //STAGE_FIGHTER_LABEL_H
