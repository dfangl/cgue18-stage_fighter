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

    Camera camera(glm::vec3(0.0f, 10.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 45.0f, config["config"]["width"], config["config"]["height"], 0.01f, 100.0f);
	auto *window = new Window(camera, config["config"]["width"], config["config"]["height"], "Stage Fighter", config["config"]["fullscreen"]);
    window->setVSync(config["config"]["vsync"]);

	auto world = std::make_shared<BulletUniverse>(btVector3(0,-9.81f,0));

	window->registerKeyCallback([window](int key, int scancode, int action, int mods){
		if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			window->close();
	});

	//TODO: Move Bullet stuff to anoter location:
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
	btRigidBody::btRigidBodyConstructionInfo
		groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	world->addRigidBody(groundRigidBody);
	//=============================

    auto cube = std::make_shared<CubeEntity>(glm::vec4(0,10,0,1), TextureManager::load("wall.jpg"), world);
    auto cube1 = std::make_shared<CubeEntity>(glm::vec4(5,10,1,1), TextureManager::load("wall.jpg"), world);
    auto cube2 = std::make_shared<CubeEntity>(glm::vec4(3,3,1,1), TextureManager::load("wall.jpg"), world);
    auto cube3 = std::make_shared<CubeEntity>(glm::vec4(1,3,4,1), TextureManager::load("wall.jpg"), world);
    auto cube4 = std::make_shared<CubeEntity>(glm::vec4(5,10,0,1), TextureManager::load("wall.jpg"), world);
    auto triangle = std::make_shared<Triangle>(glm::vec4(0,1,0,1), TextureManager::load("wall.jpg"));
    auto player = std::make_shared<CameraEntity>(camera, world);
    auto cubeModel = std::make_shared<Model3DObject>(ModelManager::load("cube"), ShaderManager::load("cube"));

    triangle->rotate(90.0f, glm::vec3(1,0,0));

    window->addObject3D(cube);
    window->addObject3D(cube1);
    window->addObject3D(cube2);
    window->addObject3D(cube3);
    window->addObject3D(cube4);
    window->addObject3D(triangle);
    window->addObject3D(cubeModel);

    auto lastTick = std::chrono::high_resolution_clock::now();

    window->processCameraMouseMovement(true);
    window->processCameraKeyMovment(false);
    player->enabled = true;
    //window->hideCursor();

    double frameSampleCount = 0.0;

    window->registerKeyCallback(player->getKeyboardCallback());
    window->registerKeyCallback([cube, player, window](int key, int scancode, int action, int mods){
        if(key == GLFW_KEY_R && action == GLFW_RELEASE)
            cube->setEntityPosition(glm::vec3(0,10,0));

        if(key == GLFW_KEY_F3 && action == GLFW_RELEASE) {
            window->processCameraKeyMovment(player->enabled);
            player->enabled = !player->enabled;
        }
    });

	while (window->isOpen())
	{
		auto curTick = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> delta =  curTick - lastTick;
		lastTick = curTick;

        player->think(delta);

		world->simulate(delta);

        // transfer bullet world to opengl
        cube->think(delta);
        cube1->think(delta);
        cube2->think(delta);
        cube3->think(delta);
        cube4->think(delta);


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

    window->showCurosor();

	world->removeRigidBody(groundRigidBody);
	delete groundRigidBody->getMotionState();
	delete groundRigidBody;

	delete groundShape;
	delete window;

    ShaderManager::destroy();
    TextureManager::destroy();

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
