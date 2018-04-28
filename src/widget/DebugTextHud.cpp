//
// Created by Raphael on 22.04.2018.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../manager/FontManager.h"

#include "DebugTextHud.h"

DebugTextHud::DebugTextHud() : Logger("DebugHUD") {
    this->fpsLabel = std::make_shared<Label>("FPS: 000.0 (00.0000 ms | 00.0000 ms highest)",
                                            FontManager::get("Lato-12"),       // Font
                                            3.0f, 15.0f,                       // X, Y Coordinates on the Screen
                                            1.0f,                              // Font scaling
                                            glm::vec3(0.9f, 0.9f, 0.9f)        // Color RGB values between 0.0 and 1.0
    );

    keyCallback = [this](int key, int scancode, int action, int mods){
        if(action == GLFW_RELEASE)
            switch(key) {
                case GLFW_KEY_F1: logger->info("Help functionality is not implemented!"); break;
                case GLFW_KEY_F2: showFrameTime = !showFrameTime; break;
                case GLFW_KEY_F3: wireFrameMode = !wireFrameMode; break;
                case GLFW_KEY_F4: toggleEffect(0); break;
                case GLFW_KEY_F5: toggleEffect(1); break;
                case GLFW_KEY_F6: toggleEffect(2); break;
                case GLFW_KEY_F7: toggleEffect(3); break;
                case GLFW_KEY_F8: viewFrustumCulling = !viewFrustumCulling; break;
                default: { /* Nope no default handling here */ }
            }
    };
}

void DebugTextHud::toggleEffect(int id) {
    logger->info("Toggled Effect: {} {}", id, effect[id] ? "off" : "on");
    effect[id] = !effect[id];
}

void DebugTextHud::render(const glm::mat4 &projection) {
    if (showFrameTime) fpsLabel->render(projection);
}

void DebugTextHud::resize(float x, float y) {}

std::function<void(int, int, int, int)> &DebugTextHud::getKeyCallback() {
    return this->keyCallback;
}

void DebugTextHud::update(std::chrono::duration<double, std::milli> delta) {
    this->refreshTimer += delta.count();
    if (highestDelta < delta.count()) highestDelta = delta.count();

    /*
     * Process FPS Counter update, but only if 250ms since the last update
     * have been passed (updating every frame is a waste of performance)
     */
    if ((refreshTimer) > 250.0) {
        const double fps = 1000.0/delta.count();
        const int len = snprintf(this->fpsBuffer, 64, "FPS: %3.1f (%2.4f ms | %2.4f ms highest)", fps, delta.count(), highestDelta);
        fpsLabel->setText(std::string(fpsBuffer, fpsBuffer+len));
        refreshTimer = 0;
        highestDelta = 0;
    }
}
