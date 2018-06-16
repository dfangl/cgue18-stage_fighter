//
// Created by Raphael on 08.04.2018.
//

#include "NuklearCtx.h"
#include "../helper/Callback.h"
#include "../manager/ShaderManager.h"

#define NK_IMPLEMENTATION
#include <nuklear.h>


NuklearContext::NuklearContext(Window *window) {
    this->win = window;
    this->shader = ShaderManager::load("nuklear");

    // TODO: window->registerScrollCallback
    // TODO: window->registerCharCallback
    // TODO: window->registerMouseBtnCallback

    nk_init_default(&this->ctx, nullptr);
    {
        {
            Callback<void(nk_handle, struct nk_text_edit *)>::func =
                    std::bind(&NuklearContext::nkHandlePaste, this, std::placeholders::_1, std::placeholders::_2);

            ctx.clip.paste = static_cast<nk_plugin_paste>(Callback<void(nk_handle, struct nk_text_edit *)>::callback);
        }
        {
            Callback<void(nk_handle, const char *, int)>::func =
                    std::bind(&NuklearContext::nkHandleCopy, this, std::placeholders::_1, std::placeholders::_2,
                              std::placeholders::_3);

            ctx.clip.copy = static_cast<nk_plugin_copy>(Callback<void(nk_handle, const char *, int)>::callback);
        }

        ctx.clip.userdata = nk_handle_ptr(nullptr);
    }

    nk_buffer_init_default(&this->cmds);
    {
        /* buffer setup */
        GLsizei vs = sizeof(struct nk_glfw_vertex);
        size_t vp = offsetof(struct nk_glfw_vertex, position);
        size_t vt = offsetof(struct nk_glfw_vertex, uv);
        size_t vc = offsetof(struct nk_glfw_vertex, col);

        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glGenVertexArrays(1, &vao);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        shader->setVertexAttributePointer("Position", 2, GL_FLOAT, GL_FALSE, vs, (void*)vp);
        shader->setVertexAttributePointer("TexCoord", 2, GL_FLOAT, GL_FALSE, vs, (void*)vt);
        shader->setVertexAttributePointer("Color", 4, GL_UNSIGNED_BYTE, GL_TRUE, vs, (void*)vc);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // TODO: integrate Font loading into Font Manager
    {
        nk_font_atlas_init_default(&atlas);
        nk_font_atlas_begin(&atlas);

        struct nk_font *regular  = nk_font_atlas_add_from_file(&atlas, "../resources/fonts/Lato-Regular.ttf", 13, nullptr);
        struct nk_font *headline = nk_font_atlas_add_from_file(&atlas, "../resources/fonts/Metamorphous-Regular.ttf", 26, nullptr);

        const void *image; int w, h;
        image = nk_font_atlas_bake(&atlas, &w, &h, NK_FONT_ATLAS_RGBA32);

        glGenTextures(1, &font_tex);
        glBindTexture(GL_TEXTURE_2D, font_tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)w, (GLsizei)h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

        nk_font_atlas_end(&atlas, nk_handle_id((int)font_tex), &this->null);
        if (atlas.default_font)
            nk_style_set_font(&this->ctx, &atlas.default_font->handle);

        nk_style_set_font(&ctx, &regular->handle);

        this->fonts.push_back(regular);
        this->fonts.push_back(headline);
    }

    // TODO: Callback to support multiple styles in multiple contexts
   this->resetStyle();
}

void NuklearContext::nkHandlePaste(nk_handle usr, struct nk_text_edit *edit) {
    const char *text = win->getClipboardString();
    if (text)
        nk_textedit_paste(edit, text, nk_strlen(text));

    (void)usr;
}

void NuklearContext::nkHandleCopy(nk_handle usr, const char *text, int len) {
    (void)usr;

    if (!len)
        return;

    auto *str = (char*)malloc((size_t)len+1);
    memcpy(str, text, (size_t)len);
    str[len] = '\0';

    win->setClipboardString(str);
    free(str);
}

NuklearContext::~NuklearContext() {
    nk_buffer_free(&this->cmds);
}

void NuklearContext::render(const glm::mat4 &projection, float screenGamma) {
    if (!enabled) {
        nk_clear(&this->ctx);
        return;
    }

    for (auto &w : this->widgets) {
        w->render();
    }

    struct nk_buffer vbuf, ebuf;
    const struct nk_draw_command *cmd;
    const nk_draw_index *offset = nullptr;

    // Prepare Environment:
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);

    shader->use();
    shader->setUniform("ProjMtx", projection);
    shader->setUniform("Texture", 0);
    shader->setUniformIfNeeded("screenGamma", screenGamma);

    // Bind VAO, VBO & EBO
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBufferData(GL_ARRAY_BUFFER, MAX_VERTEX_BUFFER, nullptr, GL_STREAM_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_ELEMENT_BUFFER, nullptr, GL_STREAM_DRAW);

    void *vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    void *elements = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
    {
        /* fill convert configuration */
        struct nk_convert_config config;
        static const struct nk_draw_vertex_layout_element vertex_layout[] = {
                {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_glfw_vertex, position)},
                {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_glfw_vertex, uv)},
                {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(struct nk_glfw_vertex, col)},
                {NK_VERTEX_LAYOUT_END}
        };
        NK_MEMSET(&config, 0, sizeof(config));
        config.vertex_layout = vertex_layout;
        config.vertex_size = sizeof(struct nk_glfw_vertex);
        config.vertex_alignment = NK_ALIGNOF(struct nk_glfw_vertex);
        config.null = this->null;
        config.circle_segment_count = 22;
        config.curve_segment_count = 22;
        config.arc_segment_count = 22;
        config.global_alpha = 1.0f;
        config.shape_AA = NK_ANTI_ALIASING_ON;
        config.line_AA = NK_ANTI_ALIASING_ON;

        /* setup buffers to load vertices and elements */
        nk_buffer_init_fixed(&vbuf, vertices, (size_t)MAX_VERTEX_BUFFER);
        nk_buffer_init_fixed(&ebuf, elements, (size_t)MAX_ELEMENT_BUFFER);
        nk_convert(&ctx, &cmds, &vbuf, &ebuf, &config);
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    nk_draw_foreach(cmd, &this->ctx, &this->cmds)
    {
        if (!cmd->elem_count) continue;
        glBindTexture(GL_TEXTURE_2D, (GLuint)cmd->texture.id);
        glScissor(
                (GLint)(cmd->clip_rect.x),
                (GLint)((win->getHeight() - (GLint)(cmd->clip_rect.y + cmd->clip_rect.h))),
                (GLint)(cmd->clip_rect.w),
                (GLint)(cmd->clip_rect.h));
        glDrawElements(GL_TRIANGLES, (GLsizei)cmd->elem_count, GL_UNSIGNED_SHORT, offset);
        offset += cmd->elem_count;
    }
    nk_clear(&this->ctx);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
    glUseProgram(0);
}

void NuklearContext::resize(float x, float y) {
    for (auto &w : this->widgets) {
        w->resize(x, y);
    }
}

void NuklearContext::newFrame() {
    if (!enabled)
        return;

    nk_input_begin(&ctx);

    double x,y;
    glfwGetCursorPos(win->getGlfwWindow(), &x, &y);
    nk_input_motion(&ctx, static_cast<int>(x), static_cast<int>(y));

    nk_input_button(&ctx, NK_BUTTON_LEFT, (int)x, (int)y, glfwGetMouseButton(win->getGlfwWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
    nk_input_button(&ctx, NK_BUTTON_MIDDLE, (int)x, (int)y, glfwGetMouseButton(win->getGlfwWindow(), GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
    nk_input_button(&ctx, NK_BUTTON_RIGHT, (int)x, (int)y, glfwGetMouseButton(win->getGlfwWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);

    nk_input_end(&ctx);
}

void NuklearContext::add(std::shared_ptr<NuklearWidget> widget) {
    widgets.push_back(widget);
}

void NuklearContext::resetStyle() {
    ctx.style.button.rounding = 0;
    ctx.style.window.fixed_background = nk_style_item_color(nk_rgba(15, 15, 15, 215));
    ctx.style.window.border = 0;
    ctx.style.button.border = 0;
    ctx.style.button.text_normal = nk_rgb(245, 245, 245);
    ctx.style.button.text_hover = nk_rgb(245, 245, 245);
    ctx.style.button.normal = nk_style_item_color(nk_rgb(66, 66, 66));
    ctx.style.text.color = nk_rgb(245, 245, 245);
}