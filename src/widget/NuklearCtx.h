//
// Created by Raphael on 08.04.2018.
//

#ifndef STAGE_FIGHTER_NUKLEARCTX_H
#define STAGE_FIGHTER_NUKLEARCTX_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <memory>
#include <functional>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include <nuklear.h>

#include "../Window.h"
#include "Widget.h"

#define MAX_VERTEX_BUFFER (512 * 1024)
#define MAX_ELEMENT_BUFFER (128 * 1024)

class NuklearWidget {
protected:
    bool visible = false;

public:
    virtual void render() = 0;
    virtual void resize(float x, float y) = 0;
    virtual void show() = 0;
    virtual void hide() = 0;

    bool isVisible() { return visible; }
};

class NuklearContext : public Widget {

private:
    struct nk_glfw_vertex {
        float position[2];
        float uv[2];
        nk_byte col[4];
    };

    GLuint vbo, vao, ebo;
    GLuint font_tex;

    struct nk_context ctx;
    struct nk_buffer cmds;
    struct nk_draw_null_texture null;
    struct nk_font_atlas atlas;
    std::vector<struct nk_font *> fonts;

    Window *win;

    std::vector<char> text;
    std::shared_ptr<Shader> shader;

    std::vector<std::shared_ptr<NuklearWidget>> widgets;

protected:
    void nkHandlePaste(nk_handle usr, struct nk_text_edit *edit);
    void nkHandleCopy(nk_handle usr, const char *text, int len);

public:
    explicit NuklearContext(Window *window);
    ~NuklearContext();

    void newFrame();
    void render(const glm::mat4 &projection) override;

    void resize(float x, float y) override;

    void add(std::shared_ptr<NuklearWidget> widget);

    struct nk_context *context() { return &this->ctx; }
    Window *window() { return this->win; }

    //void hideAll();

    bool enabled = true;

    const std::vector<struct nk_font *> &getFonts() const { return this->fonts; };
    void clear() { this->widgets.clear(); }
};

#endif //STAGE_FIGHTER_NUKLEARCTX_H
