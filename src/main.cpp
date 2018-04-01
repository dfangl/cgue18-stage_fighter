#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <chrono>
#include <memory>

#include <btBulletDynamicsCommon.h>


#include "Window.h"
#include "BulletUniverse.h"
#include "object3d/Triangle.h"
#include "manager/ShaderManager.h"
#include "manager/TextureManager.h"
#include "object3d/Cube.h"
#include "entity/CubeEntity.h"
#include "controller/CameraController.h"
#include "manager/ModelManager.h"
#include "object3d/Model3DObject.h"
#include "object3d/StaticBulletModelObject.h"
#include "level/Level.h"
#include "manager/FontManager.h"
#include "widget/Label.h"

#include <kaguya/kaguya.hpp>
#include <spdlog/spdlog.h>

// Needed for one time implementation, do NOT delete
#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

int main(int argc, char *argv[]) {
    // Setup Logger:
    spdlog::set_async_mode(8192);
    auto console = spdlog::stdout_color_mt("console");
    console->info("Loading ../config.lua");

    // Load configuration file:
	kaguya::State config;
    config.dofile("../config.lua");

    // Initialize "smart" resource management stuff":
	console->info("Shader root is ../resources/shader");
    ShaderManager::build("../resources/shader/");
    TextureManager::build("../resources/texture/");
    FontManager::build("../resources/fonts/");
    ModelManager::build("../resources/");
    //Lato-Regular

    // Create Window and Camera system:
    Camera camera(glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                  -90.0f, 0.0f,
                  config["camera"]["fov"], config["config"]["width"], config["config"]["height"],
                  0.01f, 1000.0f
    );
	auto *window = new Window(camera, config["config"]["width"], config["config"]["height"], "Stage Fighter",
                              config["config"]["fullscreen"]
    );

	// Configure Window Stuff:
    window->setVSync(config["config"]["vsync"]);
    // TODO: Should be removed -> ESC window closes
    window->registerKeyCallback([window](int key, int scancode, int action, int mods){
        if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            window->close();
    });

    // Create Bullet World and load the Test level:
	auto world = std::make_shared<BulletUniverse>(btVector3(0,-9.81f,0));
    auto level = std::make_shared<Level>("../resources/level/test.lua", world);

    // When debugging bullet!
    bool bulletDbgFlag = config["debug"]["bullet"];
    if (bulletDbgFlag){
        world->enableDebugging();
        window->addObject3D(world->getDebugDrawer());
    }

    // Prepare level and window ...
    level->start(camera, window);
    window->processCameraMouseMovement(true);
    window->processCameraKeyMovment(false);
    window->hideCursor();

    FontManager::load("Lato-Regular")->setSize(48);
    auto t = std::string("\"Test text\"");
    auto text = std::make_shared<Label>(t, FontManager::load("Lato-Regular"), 520.0f, 570.0f, 1.0f, glm::vec3(0.3f, 0.7f, 0.9f));
    window->addWidget(text);

    // Enter main game Loop:
    auto lastTick = std::chrono::high_resolution_clock::now();
    double frameSampleCount = 0.0;
    bool openGlDbgFlag = config["debug"]["opengl"];

	while (window->isOpen()) {
	    // Timing, delta from frames
		auto curTick = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> delta =  curTick - lastTick;
		lastTick = curTick;

		// Process Bullet physics and debug renderer
		world->simulate(delta);
        if (bulletDbgFlag) world->drawDebug();

        // Process level stuff:
        level->tick(delta);

        // Process fps counter
        // TODO: move into window class (?)
        if ((frameSampleCount+=delta.count()) > 1000.0) {
            console->info("tick time: {}\t fps: {}", delta.count(), 1000.0/delta.count());
            frameSampleCount = 0;
        }

        // Finally render Window content:
        window->render(delta);

        // Process the glGetError function, but only
        // if requested in the config file
        if (openGlDbgFlag) {
            auto error = glGetError();
            if(error != GL_NO_ERROR) {
                console->error("OpenGL Error Code: {}", error);
            }
        }
	}

	// Destroy all the Stuff we created:
	level->destroy();
    window->showCurosor();
    window->removeWidget(text);
	delete window;

    ShaderManager::destroy();
    TextureManager::destroy();
    ModelManager::destroy();
    FontManager::destroy();

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
