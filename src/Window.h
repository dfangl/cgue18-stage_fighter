//
// Created by raphael on 13.03.18.
//

#ifndef STAGE_FIGTHER_WINDOW_H
#define STAGE_FIGTHER_WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <functional>
#include <memory>

#include "Camera.h"
#include "Scene.h"

#include "object3d/Object3D.h"
#include "widget/Widget.h"

/**
 *  Window Class which represents a Window on the Screen, this class does hold all the callbacks and the Camera, besides
 *  that it also supports a simple free camera control
 */
class Window : public Logger {

private:
    GLFWwindow* glfwWindow;
    GLFWmonitor* monitor;
    int width, height;

    unsigned int callbackID = 0;
    std::map<unsigned int, std::function<void(double, double)>> mouseCallbacks;
    std::map<unsigned int, std::function<void(int, int, int, int)>> keyInputCallbacks;
    std::map<unsigned int, std::function<void(Window const *)>> inputPollCallbacks;
    std::map<unsigned int, std::function<void(double, double)>> scrollCallback;

    std::shared_ptr<Scene> scene;
    std::vector<std::shared_ptr<Widget>> widgets;

    glm::mat4 widgetProjectionMatrix;

    double oldXCursorPosition = 0.0;
    double oldYCursorPosition = 0.0;

    bool cameraMouse = false;
    bool cameraKey = false;

    float screenGamma = 1.0f;

public:
    /**
    * Create a Window with GLFW with the specific size, the glfw library must already be initalized!
    * @param width of the window in px
    * @param height height of the window in px
    * @param windowName the name of the window
    * @param fullscreen -1 if windowed mode, otherwise the monitor index on which fullscreen is requested
    * @param refreshRate either GLFW_DONT_CARE or the refresh rate which should be used in fullscreen
    */
    Window(Camera &camera, int width, int height, const std::string &windowName,
           int fullscreen = -1, int refreshRate = GLFW_DONT_CARE, bool debugContext = GL_FALSE);
    ~Window();

    void setVSync(bool enabled);

    inline int getHeight() { return this->height; }
    inline int getWidth() { return this->width; }

    void render(std::chrono::duration<double, std::milli> &delta);

    void setScene(std::shared_ptr<Scene> &scene);
    std::shared_ptr<Scene> getScene() { return this->scene; }

    void addWidgetTop(const std::shared_ptr<Widget> &widget);
    void addWidget(const std::shared_ptr<Widget> &widget);
    void removeWidget(const std::shared_ptr<Widget> &widget);

    unsigned int registerMouseCallback(std::function<void(double,double)> callback);
    unsigned int registerKeyCallback(std::function<void(int, int, int, int)> callback);
    void removeKeyCallback(unsigned int callback);

    unsigned int registerKeyPollingCallback(std::function<void(Window const *)> callback);
    void removeKeyPollingCallback(unsigned int callback);

    bool isOpen();
    void close();

    void hideCursor();
    void showCursor();

    void processCameraMouseMovement(bool locked) { this->cameraMouse = locked; }
    void processCameraKeyMovement(bool locked) { this->cameraKey = locked; }

    inline int getKey(int keycode) const { return glfwGetKey(this->glfwWindow, keycode); }
    inline int getMouseButton(int btn) const { return glfwGetMouseButton(this->glfwWindow, btn); }
    inline GLFWwindow *getGlfwWindow() const { return this->glfwWindow; }

    void setClipboardString(const char *text) const;
    const char *getClipboardString() const;

    /**
     * Gamma value should only be set if canSetGamma is true, this
     * function uses glfw in the background.
     *
     * @param gamma between 0.0 and + infinity
     */
    void setGamma(float gamma);
    bool canSetGamma() const { return monitor != nullptr; }

    void requestFocus();

    /**
     * Sets gamma correction but needs Shader support instead of GLFW support
     * @param gamma between 0.0 and + infinity
     */
    void setScreenGamma(float gamma);
    float getScreenGamma() { return this->screenGamma; }

    // Stuff needed for C API, don't call directly
    void glfwWindowSizeChanged(int width, int height);
    void glfwMouseCallabck(double xpos, double ypos);
    void glfwKeyCallback(int key, int scancode, int action, int mods);
    void glfwMouseButtonCallback(int button, int action, int mods);
    void glfwScrollCallback(double xoffset, double yoffset);
};


#endif //STAGE_FIGTHER_WINDOW_H
