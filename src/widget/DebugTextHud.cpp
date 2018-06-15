//
// Created by Raphael on 22.04.2018.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "DebugTextHud.h"

#include "../manager/FontManager.h"
#include "../manager/ShaderManager.h"

#include "../helper/CompilerMacros.h"

DebugTextHud::DebugTextHud(const std::shared_ptr<Scene> &scene) : Logger("DebugHUD"), scene(scene) {
    this->fpsLabel = std::make_shared<Label>("FPS: 000.0 (00.0000 ms | 00.0000 ms highest)",
                                            FontManager::get("Lato-12"),       // Font
                                            3.0f, 15.0f,                       // X, Y Coordinates on the Screen
                                            1.0f,                              // Font scaling
                                            glm::vec3(0.9f, 0.9f, 0.9f)        // Color RGB values between 0.0 and 1.0
    );

    this->viewFrustumCulling = scene->frustumCulling;
    this->cullingLabel = std::make_shared<Label>("", FontManager::get("Lato-12"), 3, 30.0f, 1.0f, glm::vec3(.9f,.9f,.9f));

    keyCallback = [this, scene](int key, int UNUSED(scancode), int action, int UNUSED(mods)){
        if(action == GLFW_RELEASE)
            switch(key) {
                case GLFW_KEY_F2: showFrameTime = !showFrameTime; break;
                case GLFW_KEY_F3: toggleWireframeMode(); break;
                case GLFW_KEY_F4: toggleEffect(0); break;
                case GLFW_KEY_F5: toggleEffect(1); break;
                case GLFW_KEY_F6: toggleEffect(2); break;
                case GLFW_KEY_F7: toggleEffect(3); break;
                case GLFW_KEY_F8: viewFrustumCulling = !viewFrustumCulling; scene->frustumCulling = viewFrustumCulling; break;
                case GLFW_KEY_F11: ShaderManager::recompileAll(); this->scene->makeLightsDirty(); break;
                default: { /* Nope no default handling here */ }
            }
    };

    std::memset(this->fpsBuffer, 0, 64);
}

void DebugTextHud::toggleEffect(int id) {
    logger->info("Toggled Effect: {} {}", id, effect[id] ? "off" : "on");
    effect[id] = !effect[id];
}

void DebugTextHud::render(const glm::mat4 &projection) {
    if (showFrameTime) {
        fpsLabel->render(projection);
        cullingLabel->render(projection);
    }
}

void DebugTextHud::resize(float UNUSED(x), float UNUSED(y)) {}

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
    if (showFrameTime && (refreshTimer) > 250.0) {
        const double fps = 1000.0/delta.count();
        int len = snprintf(this->fpsBuffer, 64, "FPS: %3.1f (%2.4f ms | %2.4f ms highest)", fps, delta.count(), highestDelta);
        fpsLabel->setText(std::string(fpsBuffer, fpsBuffer+len));

        if (viewFrustumCulling) len = snprintf(this->fpsBuffer, 64, "Culled Objects: %u", scene->getCulledObjectCount());
        else                    len = snprintf(this->fpsBuffer, 64, "Culled Objects: [Disabled - F8]");
        cullingLabel->setText(std::string(fpsBuffer, fpsBuffer+len));

        refreshTimer = 0;
    }

    highestReset -= delta.count();
    if ( highestReset < 0){
        highestDelta = 0;
        highestReset = 60000.0;
    }
}

void DebugTextHud::toggleWireframeMode() {
    wireFrameMode = !wireFrameMode;

    if(wireFrameMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else              glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
