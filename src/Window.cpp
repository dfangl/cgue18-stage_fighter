//
// Created by raphael on 13.03.18.
//

#include "Window.h"
#include "helper/Callback.h"

#include <algorithm>

typedef void (*framebuffer_size_callback)(GLFWwindow*,int,int);
typedef void (*mouse_callback)(GLFWwindow*,double,double);
typedef void (*scroll_callback)(GLFWwindow*,double,double);

#include <iostream>
static void glfw_error_callack_func(int code, const char * reason) {
	std::cout << "[Error: GLFW] " << code << " with " << reason << std::endl;
}

Window::Window(int width, int height, const std::string &windowName, bool fullscreen) {
    this->height = height;
    this->width = width;

    // Init GLFW Library:
    if(!glfwInit()) {
        throw std::runtime_error("Unable to init GLFW!");
    }

	glfwSetErrorCallback(glfw_error_callack_func);

    // Set OpenGL Context to 4.5 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor *monitor = nullptr;
    if(fullscreen)
        monitor = glfwGetPrimaryMonitor();

    this->glfwWindow = glfwCreateWindow(width, height, windowName.c_str(), monitor, nullptr);
    if (this->glfwWindow == nullptr) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW Window!");
    }

    glfwMakeContextCurrent(this->glfwWindow);

    // Do some black witchery because C++11 and C don't mix function pointer well
    {
        Callback<void(GLFWwindow *, int, int)>::func = std::bind(&Window::glfwWindowSizeChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        auto framebufferCallbackFunc = static_cast<framebuffer_size_callback>(Callback<void(GLFWwindow*,int,int)>::callback);

        glfwSetFramebufferSizeCallback(this->glfwWindow, framebufferCallbackFunc);
    }

    {
        Callback<void(GLFWwindow*,double,double)>::func =
                std::bind(&Window::glfwMouseCallabck, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        auto mouseCallbackFunc = static_cast<mouse_callback>(Callback<void(GLFWwindow*,double,double)>::callback);

        glfwSetCursorPosCallback(this->glfwWindow, mouseCallbackFunc);
    }

    {
        //Callback<void(GLFWwindow*,double,double)>::func =
        //        std::bind(&Window::glfwScrollCallbakc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        //auto scrollCallbackFunc = static_cast<scroll_callback>(Callback<void(GLFWwindow*,double,double)>::callback);

        //glfwSetScrollCallback(this->glfwWindow, scrollCallbackFunc);
    }

    //glfwSetInputMode(this->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //Load GLAD Stuff:
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }
}

Window::~Window() {
    // Destroy the window:
    glfwDestroyWindow(this->glfwWindow);
}

void Window::setVSync(bool enabled) {
    glfwSwapInterval(enabled ? 1 : 0);
}

void Window::glfwWindowSizeChanged(GLFWwindow* window,int width, int height) {
    this->height = height;
    this->width = width;

    // Resize OpenGL Viewport to new Window Size
    glViewport(0, 0, width, height);
}

void Window::glfwMouseCallabck(GLFWwindow* window, double xpos, double ypos) {
    for (auto &callback : this->mouseCallbacks) {
        callback(xpos, ypos);
    }
}

bool Window::isOpen() {
    return !glfwWindowShouldClose(this->glfwWindow);
}

void Window::close() {
    glfwSetWindowShouldClose(this->glfwWindow, true);
}

void Window::registerMouseCallback(std::function<void(double, double)> callback) {
    this->mouseCallbacks.push_back(callback);
}

void Window::registerKeyProcessor(std::function<void()> callback) {
    this->keyInputCallbacks.push_back(callback);
}

void Window::render(std::function<void()> func) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwPollEvents();
    for(auto &callback : this->keyInputCallbacks) {
        callback();
    }

    func();

    for(auto &obj : this->objects) {
        obj->render();
    }

    glfwSwapBuffers(this->glfwWindow);
}

void Window::addObject3D(const std::shared_ptr<Object3D> &object3D) {
    this->objects.push_back(object3D);
}

void Window::removeObject(const std::shared_ptr<Object3D> &object3D) {
    this->objects.erase(
            std::remove_if(
                    this->objects.begin(),
                    this->objects.end(),
                    [object3D](std::shared_ptr<Object3D> current) -> bool {
                        return current == object3D;
                    }
            ),
            this->objects.end()
    );
}
