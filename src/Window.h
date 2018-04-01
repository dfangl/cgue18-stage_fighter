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

#include "object3d/Object3D.h"
#include "Camera.h"
#include "widget/Widget.h"

class Window : public Logger {

private:
    GLFWwindow* glfwWindow;
    int width, height;

    std::vector<std::function<void(double, double)>> mouseCallbacks;
    std::vector<std::function<void(int, int, int, int)>> keyInputCallbacks;
    std::vector<std::function<void(Window const *)>> inputPollCallbacks;

    std::vector<std::shared_ptr<Object3DAbstract>> objects;
    std::vector<std::shared_ptr<Widget>> widgets;

    Camera &camera;
    glm::mat4 widgetProjectionMatrix;

    double oldXCursorPosition = 0.0;
    double oldYCursorPosition = 0.0;

    bool cameraMouse = false;
    bool cameraKey = false;

protected:
    void glfwWindowSizeChanged(GLFWwindow* window,int width, int height);
    void glfwMouseCallabck(GLFWwindow* window, double xpos, double ypos);
    void glfwErrorCallabck(int code, const char *text);
    void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    //void glfwScrollCallbakc(GLFWwindow* window, double xoffset, double yoffset);

public:
    /**
    * Create a Window with GLFW with the specific size
    * @param width of the window in px
    * @param height height of the window in px
    * @param windowName the name of the window
    * @param fullscreen true if fullscreen mode is desired
    */
    Window(Camera &camera, int width, int height, const std::string &windowName, bool fullscreen = false);
    ~Window();

    void setVSync(bool enabled);

    inline int getHeight() { return this->height; }
    inline int getWidth() { return this->width; }

    void render(std::chrono::duration<double, std::milli> delta);

    void addObject3D(const std::shared_ptr<Object3DAbstract> &object3D);
    void removeObject(const std::shared_ptr<Object3DAbstract> &object3D);
    
    void addWidget(const std::shared_ptr<Widget> &widget);
    void removeWidget(const std::shared_ptr<Widget> &widget);

    int registerMouseCallback(std::function<void(double,double)> callback);
    int registerKeyCallback(std::function<void(int, int, int, int)> callback);
    int registerKeyPollingCallback(std::function<void(Window const *)> callback);
    void removeKeyPollingCallback(int callback);

    bool isOpen();
    void close();

    void hideCursor();
    void showCurosor();

    void processCameraMouseMovement(bool locked) { this->cameraMouse = locked; }
    void processCameraKeyMovment(bool locked) { this->cameraKey = locked; }

    inline int getKey(int keycode) const { return glfwGetKey(this->glfwWindow, keycode); }
    inline Camera &getCamera() const { return this->camera; }

};


#endif //STAGE_FIGTHER_WINDOW_H
