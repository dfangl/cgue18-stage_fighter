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

#include <kaguya/kaguya.hpp>
#include <spdlog/spdlog.h>

// Needed for one time implementation, do NOT delete
#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>


int main(int argc, char *argv[]) {
    spdlog::set_async_mode(8192);
    auto console = spdlog::stdout_color_mt("console");
    console->info("Loading ../config.lua");

	kaguya::State config;
    config.dofile("../config.lua");

	console->info("Shader root is ../resources/shader");
    ShaderManager::build("../resources/shader/");
    TextureManager::build("../resources/texture/");
    ModelManager::build("../resources/");

    Camera camera(glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, config["camera"]["fov"], config["config"]["width"], config["config"]["height"], 0.01f, 1000.0f);
	auto *window = new Window(camera, config["config"]["width"], config["config"]["height"], "Stage Fighter", config["config"]["fullscreen"]);
    window->setVSync(config["config"]["vsync"]);

	auto world = std::make_shared<BulletUniverse>(btVector3(0,-9.81f,0));

	window->registerKeyCallback([window](int key, int scancode, int action, int mods){
		if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			window->close();
	});
	
    // Load level data:
    auto level = std::make_shared<Level>("../resources/level/test.lua", world);

    // ONLY when debugging bullet!
    world->enableDebugging();
    window->addObject3D(world->getDebugDrawer());

    auto lastTick = std::chrono::high_resolution_clock::now();

    window->processCameraMouseMovement(true);
    window->processCameraKeyMovment(false);
    window->hideCursor();

    double frameSampleCount = 0.0;

    /*
    window->registerKeyCallback([console, player, window](int key, int scancode, int action, int mods){
        if (key == GLFW_KEY_F3 && action == GLFW_RELEASE) {
            window->processCameraKeyMovment(player->enabled);
            player->enabled = !player->enabled;
        }
        if (key == GLFW_KEY_F4 && action == GLFW_RELEASE) {
            auto p = player->getPosition();
            console->info("Camera Position: {} {} {}", p.x, p.y, p.z);
        }
    });
    */

    level->start(camera, window);

	while (window->isOpen())
	{
		auto curTick = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> delta =  curTick - lastTick;
		lastTick = curTick;

		world->simulate(delta);
        world->drawDebug();

        level->tick(delta);

        if ((frameSampleCount+=delta.count()) > 1000.0) {
            console->info("tick time: {}\t fps: {}", delta.count(), 1000.0/delta.count());
            frameSampleCount = 0;
        }

        window->render(delta);

        //TODO: should only be present if debug build
		auto error = glGetError();
		if(error != GL_NO_ERROR) {
			console->error("OpenGL Error Code: {}", error);
		}

	}

	level->destroy();
    window->showCurosor();
	delete window;

    ShaderManager::destroy();
    TextureManager::destroy();
    ModelManager::destroy();

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
