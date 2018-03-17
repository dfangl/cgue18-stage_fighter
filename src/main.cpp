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

#include <kaguya/kaguya.hpp>
#include <spdlog/spdlog.h>

int main(int argc, char *argv[]) {
    spdlog::set_async_mode(8192);
    auto console = spdlog::stdout_color_mt("console");
    console->info("Loading ../config.lua");

	kaguya::State config;
    config.dofile("../config.lua");

	console->info("Shader root is ../resources/shader");
    ShaderManager::build("../resources/shader/");
    TextureManager::build("../resources/texture/");

    Camera camera(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 45.0f, config["config"]["width"], config["config"]["height"], 0.01f, 100.0f);
	auto *window = new Window(camera, config["config"]["width"], config["config"]["height"], "Stage Fighter");
    window->setVSync(config["config"]["vsync"]);

	BulletUniverse *world = new BulletUniverse(btVector3(0,-10,0));

	window->registerKeyCallback([window](int key, int scancode, int action, int mods){
		if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			window->close();
	});

	//TODO: Move Bullet stuff to anoter location:
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	btCollisionShape* fallShape = new btSphereShape(1);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
	btRigidBody::btRigidBodyConstructionInfo
		groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	world->addRigidBody(groundRigidBody);


	btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 0)));
	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	fallShape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
	btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
	world->addRigidBody(fallRigidBody);
	//=============================

    auto triangle = std::make_shared<Cube>(glm::vec4(0,0,-5,1), TextureManager::load("wall.jpg"));
    window->addObject3D(triangle);

    auto lastTick = std::chrono::high_resolution_clock::now();
    btTransform trans;

    window->processCameraModifications(true);
    window->hideCursor();

	while (window->isOpen())
	{
		auto curTick = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> delta =  curTick - lastTick;

        // Skip frame, too fast ...
        //if (delta.count() < 0.001)
        //    continue;

		lastTick = curTick;

		world->simulate(delta);
        triangle->rotate(static_cast<float>(glfwGetTime() * delta.count()), glm::vec3(0.0f, 0.0f, 1.0f));

		fallRigidBody->getMotionState()->getWorldTransform(trans);
		//std::cout << "sphere height: " << trans.getOrigin().getY() << std::endl;
		std::cout << "tick time: " << delta.count() << std::endl;

		window->render(delta);

		auto error = glGetError();
		if(error != GL_NO_ERROR) {
			console->error("OpenGL Error Code: {}", error);
		}

	}

    window->showCurosor();

	world->removeRigidBody(fallRigidBody);
	delete fallRigidBody->getMotionState();
	delete fallRigidBody;

	world->removeRigidBody(groundRigidBody);
	delete groundRigidBody->getMotionState();
	delete groundRigidBody;


	delete fallShape;
	delete groundShape;

	delete world;
	delete window;

    ShaderManager::destroy();
    TextureManager::destroy();

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
