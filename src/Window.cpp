//
// Created by raphael on 13.03.18.
//

#include "Window.h"
#include "helper/CompilerMacros.h"

#include <algorithm>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

void framebuffer_size_callback(GLFWwindow *, int, int);
void mouse_callback(GLFWwindow *, double, double);
void error_callback(int, const char *);
void key_callback(GLFWwindow *, int, int, int, int);
void scroll_callback(GLFWwindow *, double, double);
void mbtn_callback(GLFWwindow *, int, int, int);

static bool errorCallbackSet = false;

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
                            GLsizei length, const GLchar *message, const void *userParam);

Window::Window(Camera &camera, int width, int height, const std::string &windowName, int fullscreen,
               int refreshRate, bool debugContext)
        : Logger("Window") {
    this->height = height;
    this->width = width;
    this->scene = std::make_shared<Scene>(camera);

    logger->info("Creating Window: {} ({}x{})", windowName, width, height);

    // Set Error callback only once
    if (!errorCallbackSet) {
        glfwSetErrorCallback(error_callback);
        errorCallbackSet = true;
    }

    // Set OpenGL Context to 4.3 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debugContext);

    int mcount;
    GLFWmonitor** monitors = glfwGetMonitors(&mcount);
    if (fullscreen > mcount) {
        logger->critical("Not enough Monitors connected to open at Monitor: {}", fullscreen);
        fullscreen = -1;
    }

    monitor = fullscreen > - 1 ? monitors[fullscreen] : nullptr;

    // Enumerate Modes:
    {
        for (int mi=0; mi<mcount; mi++) {
            int count;
            const GLFWvidmode* modes = glfwGetVideoModes(monitors[mi], &count);
            logger->info("Monitor: {}", glfwGetMonitorName(monitors[mi]));

            for (int i=0; i<count; i++) {
                logger->info("\tMode: {}x{} @ {}Hz, ({},{},{})", modes[i].width, modes[i].height, modes[i].refreshRate,
                             modes[i].redBits, modes[i].greenBits, modes[i].blueBits);
            }
        }

    }

    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_REFRESH_RATE, refreshRate);
    glfwWindowHint(GLFW_DECORATED, fullscreen == -1 ? GLFW_TRUE : GLFW_FALSE);

    this->glfwWindow = glfwCreateWindow(width, height, windowName.c_str(), monitor, nullptr);
    if (this->glfwWindow == nullptr) {
        glfwTerminate();
        logger->error("Failed to create GLFW Window!");
        throw std::runtime_error("Failed to create GLFW Window!");
    }

    // Initalize Callbacks the old C style (no magic C++11 syntax stuff)
    glfwMakeContextCurrent(this->glfwWindow);
    glfwSetWindowUserPointer(this->glfwWindow, this);
    glfwSetFramebufferSizeCallback(this->glfwWindow, framebuffer_size_callback);
    glfwSetCursorPosCallback(this->glfwWindow, mouse_callback);
    glfwSetKeyCallback(this->glfwWindow, key_callback);
    glfwSetScrollCallback(this->glfwWindow, scroll_callback);
    glfwSetMouseButtonCallback(this->glfwWindow, mbtn_callback);

    //Load GLAD Stuff:
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    this->widgetProjectionMatrix = glm::ortho(0.0f, (float)width, (float)height, 0.0f);

    // Lazy initialize Opengl in Scene
    this->scene->initOpenGLContext();

    // Lazy init Opengl Debug context
    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        logger->info("OpenGL Context has DEBUG Bit set, requesting debug output");

        // initialize debug output
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, this->logger.get());
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
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

void Window::glfwWindowSizeChanged(int width, int height) {
    // Happens if Fullscreen Window is minimized, glm does not like this
    if (width == 0 || height == 0)
        return;

    this->height = height;
    this->width = width;

    logger->info("Resizing window to {}x{}", width, height);

    // Resize Camera and such stuff:
    this->scene->getCamera().screenSizeChanged(width, height);
    this->widgetProjectionMatrix = glm::ortho(0.0f, (float)width, (float)height, 0.0f);

    // Resize widgets
    for( auto &w : this->widgets) {
        w->resize(width, height);
    }

    // Resize OpenGL Viewport to new Window Size
    glViewport(0, 0, width, height);
}

void Window::glfwMouseCallabck(double xpos, double ypos) {
    for (auto &callback : this->mouseCallbacks) {
        callback.second(xpos, ypos);
    }
}

bool Window::isOpen() {
    return !glfwWindowShouldClose(this->glfwWindow);
}

void Window::close() {
    glfwSetWindowShouldClose(this->glfwWindow, true);
}

unsigned int Window::registerMouseCallback(std::function<void(double, double)> callback) {
    this->mouseCallbacks[callbackID++] = callback;
    return callbackID-1;
}

unsigned int Window::registerKeyCallback(std::function<void(int, int, int, int)> callback) {
    this->keyInputCallbacks[callbackID++] = callback;
    return callbackID-1;
}

void Window::render(std::chrono::duration<double, std::milli> &delta) {
    // Clear Window before drawing
    //glClearColor(0.63f, 0.79f, 0.94f, 1.0f);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*
     * Process all callbacks which do want to poll input states such a key states
     */
    for (auto &callback : this->inputPollCallbacks) {
        callback.second(this);
    }

    // Process Camera
    if(this->cameraKey) {
        auto &camera = this->scene->getCamera();

        if(glfwGetKey(this->glfwWindow, GLFW_KEY_W) == GLFW_PRESS) camera.processKeyInput(delta, Camera::FORWARD);
        if(glfwGetKey(this->glfwWindow, GLFW_KEY_A) == GLFW_PRESS) camera.processKeyInput(delta, Camera::LEFT);
        if(glfwGetKey(this->glfwWindow, GLFW_KEY_S) == GLFW_PRESS) camera.processKeyInput(delta, Camera::RIGHT);
        if(glfwGetKey(this->glfwWindow, GLFW_KEY_D) == GLFW_PRESS) camera.processKeyInput(delta, Camera::BACKWARD);
    }

    /*
     * Process Camera movement for the free camera
     */
    if(this->cameraMouse) {
        double xPos, yPos;
        glfwGetCursorPos(this->glfwWindow, &xPos, &yPos);

        this->scene->getCamera().processMouseInput(delta, xPos-oldXCursorPosition, yPos-oldYCursorPosition);
        oldXCursorPosition = xPos;
        oldYCursorPosition = yPos;
    }

    // Enable Depth test for drawing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Render all Objects to the Window
    this->scene->render(delta);

    // Models don't disable backface culling if they enabled it
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    // Render all Widgets:
    for (auto &widget : this->widgets) {
        widget->render(this->widgetProjectionMatrix, screenGamma);
    }

    // Swap Buffers and listen to events (as GLFW suggests)
    glfwSwapBuffers(this->glfwWindow);
    glfwPollEvents();
}

void Window::glfwKeyCallback(int key, int scancode, int action, int mods) {
    for(auto &callback : this->keyInputCallbacks) {
        callback.second(key, scancode, action, mods);
    }
}

void Window::hideCursor() {
    glfwSetInputMode(this->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::showCursor() {
    glfwSetInputMode(this->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

unsigned int Window::registerKeyPollingCallback(std::function<void(Window const *)> callback) {
    this->inputPollCallbacks[callbackID++] = callback;
    return callbackID-1;
}

void Window::removeKeyPollingCallback(unsigned int callback) {
    this->inputPollCallbacks.erase(callback);
}

void Window::addWidget(const std::shared_ptr<Widget> &widget) {
    this->widgets.push_back(widget);
}

void Window::removeWidget(const std::shared_ptr<Widget> &widget) {
    this->widgets.erase(
            std::remove_if(
                    this->widgets.begin(),
                    this->widgets.end(),
                    [widget](std::shared_ptr<Widget> current) -> bool {
                        return current == widget;
                    }
            ),
            this->widgets.end()
    );
}

void Window::glfwMouseButtonCallback(int UNUSED(button), int UNUSED(action), int UNUSED(mods)) {
    // TODO: implement me
    //logger->info("Mouse Button: {}, action: {}", button, action);
}

void Window::glfwScrollCallback(double UNUSED(xoffset), double UNUSED(yoffset)) {
    // TODO: implement me
    //logger->info("Scroll: {}x{}", xoffset, yoffset);
}

void Window::setClipboardString(const char *text) const {
    glfwSetClipboardString(this->glfwWindow, text);
}

const char *Window::getClipboardString() const {
    return glfwGetClipboardString(this->glfwWindow);
}

void Window::setGamma(float gamma) {
    logger->info("Setting Gamma to {}", gamma);
    glfwSetGamma(this->monitor, gamma);
}

void Window::setScene(std::shared_ptr<Scene> &scene) {
    this->scene = scene;
    this->scene->gamma = this->screenGamma;
}

void Window::setScreenGamma(float gamma) {
    this->screenGamma = gamma;
    this->scene->gamma = screenGamma;
}

void Window::requestFocus() {
    glfwFocusWindow(this->glfwWindow);
}

void Window::removeKeyCallback(unsigned int callback) {
    this->keyInputCallbacks.erase(callback);
}

void Window::addWidgetTop(const std::shared_ptr<Widget> &widget) {
    this->widgets.insert(widgets.begin(), widget);
}


// ======================
// C GFLW Proxy Handlers:
// ======================

void framebuffer_size_callback(GLFWwindow *window, int w, int h) {
    static_cast<Window*>(glfwGetWindowUserPointer(window))->glfwWindowSizeChanged(w, h);
}

void mouse_callback(GLFWwindow *window, double x, double y) {
    static_cast<Window *>(glfwGetWindowUserPointer(window))->glfwMouseCallabck(x, y);
}

void error_callback(int errorCode, const char *message) {
    spdlog::get("Window")->error("({}): {}", errorCode, message);
}

void key_callback(GLFWwindow *window, int a, int b, int c, int d) {
    static_cast<Window *>(glfwGetWindowUserPointer(window))->glfwKeyCallback(a, b, c, d);
}

void scroll_callback(GLFWwindow *window, double x, double y) {
    static_cast<Window *>(glfwGetWindowUserPointer(window))->glfwScrollCallback(x, y);

}

void mbtn_callback(GLFWwindow *window, int a, int b, int c) {
    static_cast<Window *>(glfwGetWindowUserPointer(window))->glfwMouseButtonCallback(a, b, c);
}


// OpenGL Debug Function:
void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei UNUSED(length),
                            const GLchar *message, const void *userParam) {
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    // extract logger:
    auto *logger = (spdlog::logger *)userParam;

    std::string sourceStr;
    switch (source) {
        case GL_DEBUG_SOURCE_API:             sourceStr = "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   sourceStr = "Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     sourceStr = "Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     sourceStr = "Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           sourceStr = "Other"; break;
        default: sourceStr = "Unknown";
    };

    std::string typeStr;
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:               typeStr = "Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  typeStr = "Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         typeStr = "Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         typeStr = "Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              typeStr = "Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          typeStr = "Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           typeStr = "Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               typeStr = "Other"; break;
        default: typeStr = "Unknown";
    };

    std::string severityStr;
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:         severityStr = "high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       severityStr = "medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          severityStr = "low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: severityStr = "notification"; break;
        default: severityStr = "Unknown"; break;
    };

    logger->warn("OpenGL [{}]: ID:{}, Source:{}, Type:{}, Message:{}", severityStr, id, sourceStr, typeStr, message);
}
