//
// Created by raphael on 13.03.18.
//

#include "Window.h"
#include "helper/Callback.h"

#include <algorithm>
#include <iostream>

typedef void (*framebuffer_size_callback)(GLFWwindow*,int,int);
typedef void (*mouse_callback)(GLFWwindow*,double,double);
typedef void (*error_callback)(int,const char *);
typedef void (*key_callback)(GLFWwindow*,int,int,int,int);

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
                            GLsizei length, const GLchar *message, const void *userParam);

Window::Window(Camera &camera, int width, int height, const std::string &windowName, bool fullscreen) : Logger("Window"), camera(camera) {
    this->height = height;
    this->width = width;
    //this->camera = camera;

    logger->info("Creating Window: {} ({}x{})", windowName, width, height);

    // Init GLFW Library:
    if(!glfwInit()) {
        logger->error("Failed to initialize GLFW!");
        throw std::runtime_error("Unable to init GLFW!");
    }

    { // Register Error Callback of GLFW
        Callback<void(int,const char *)>::func = std::bind(&Window::glfwErrorCallabck, this, std::placeholders::_1, std::placeholders::_2);
        auto errorCallbackFunc = static_cast<error_callback>(Callback<void(int,const char *)>::callback);

        glfwSetErrorCallback(errorCallbackFunc);
    }

    // Set OpenGL Context to 4.5 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    /*glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        // initialize debug output
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
     */

    GLFWmonitor *monitor = nullptr;
    if(fullscreen)
        monitor = glfwGetPrimaryMonitor();

    this->glfwWindow = glfwCreateWindow(width, height, windowName.c_str(), monitor, nullptr);
    if (this->glfwWindow == nullptr) {
        glfwTerminate();
        logger->error("Failed to create GLFW Window!");
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
        Callback<void(GLFWwindow*,int,int,int,int)>::func =
                std::bind(&Window::glfwKeyCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3
                        , std::placeholders::_4, std::placeholders::_5);
        auto keyCallbackFunc = static_cast<key_callback >(Callback<void(GLFWwindow*,int,int,int,int)>::callback);

        glfwSetKeyCallback(this->glfwWindow, keyCallbackFunc);
    }

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
    logger->info("{} Vsync", (enabled ? "Enabling" : "Disabling"));
    glfwSwapInterval(enabled ? 1 : 0);
}

void Window::glfwWindowSizeChanged(GLFWwindow* window,int width, int height) {
    // Happens if Fullscreen Window is minimized, glm does not like this
    if (width == 0 || height == 0)
        return;

    this->height = height;
    this->width = width;

    logger->info("Resizing window to {}x{}", width, height);

    // Resize OpenGL Viewport to new Window Size
    glViewport(0, 0, width, height);
    this->camera.screenSizeChanged(width, height);
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

void Window::registerKeyCallback(std::function<void(int, int, int, int)> callback) {
    this->keyInputCallbacks.push_back(callback);
}

void Window::render(std::chrono::duration<double, std::milli> delta) {
    // Clear Window before drawing
    glClearColor(0.63f, 0.79f, 0.94f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto &callback : this->inputPollCallbacks) {
        callback(this);
    }

    // Process Camera
    // TODO: User should be able to modify input keys:
    if(this->cameraKey) {
        if(glfwGetKey(this->glfwWindow, GLFW_KEY_W) == GLFW_PRESS) this->camera.processKeyInput(delta, Camera::FORWARD);
        if(glfwGetKey(this->glfwWindow, GLFW_KEY_A) == GLFW_PRESS) this->camera.processKeyInput(delta, Camera::LEFT);
        if(glfwGetKey(this->glfwWindow, GLFW_KEY_S) == GLFW_PRESS) this->camera.processKeyInput(delta, Camera::RIGHT);
        if(glfwGetKey(this->glfwWindow, GLFW_KEY_D) == GLFW_PRESS) this->camera.processKeyInput(delta, Camera::BACKWARD);
    }

    if(this->cameraMouse) {
        double xPos, yPos;
        glfwGetCursorPos(this->glfwWindow, &xPos, &yPos);

        this->camera.processMouseInput(delta, xPos-oldXCursorPosition, yPos-oldYCursorPosition);
        oldXCursorPosition = xPos;
        oldYCursorPosition = yPos;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Render all Objects to the Window
    for(auto &obj : this->objects) {
        obj->render(this->camera);
    }

    // Swap Buffers and listen to events (as GLFW suggests)
    glfwSwapBuffers(this->glfwWindow);
    glfwPollEvents();
}

void Window::addObject3D(const std::shared_ptr<Object3DAbstract> &object3D) {
    this->objects.push_back(object3D);
}

void Window::removeObject(const std::shared_ptr<Object3DAbstract> &object3D) {
    this->objects.erase(
            std::remove_if(
                    this->objects.begin(),
                    this->objects.end(),
                    [object3D](std::shared_ptr<Object3DAbstract> current) -> bool {
                        return current == object3D;
                    }
            ),
            this->objects.end()
    );
}

void Window::glfwErrorCallabck(int code, const char *text) {
    logger->error("{}: {}", code, text);
}

void Window::glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    for(auto &callback : this->keyInputCallbacks) {
        callback(key, scancode, action, mods);
    }
}

void Window::hideCursor() {
    glfwSetInputMode(this->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::showCurosor() {
    glfwSetInputMode(this->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::registerKeyPollingCallback(std::function<void(Window const *)> callback) {
    this->inputPollCallbacks.push_back(callback);
}

void APIENTRY glDebugOutput(GLenum source,
                            GLenum type,
                            GLuint id,
                            GLenum severity,
                            GLsizei length,
                            const GLchar *message,
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}