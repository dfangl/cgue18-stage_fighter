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

	auto *window = new Window(config["config"]["height"], config["config"]["width"], "Stage Fighter");
    window->setVSync(config["config"]["vsync"]);

	BulletUniverse *world = new BulletUniverse(btVector3(0,-10,0));

	window->registerKeyProcessor([window]{
		if(window->getKey(GLFW_KEY_ESCAPE) == GLFW_PRESS)
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

    auto triangle = std::make_shared<Triangle>(glm::vec4(0,0,0,0));
    window->addObject3D(triangle);

    auto lastTick = std::chrono::high_resolution_clock::now();
	while (window->isOpen())
	{
		auto curTick = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> delta =  curTick - lastTick;
		lastTick = curTick;

		window->render([delta, console, world, fallRigidBody]{
            btTransform trans;

            world->simulate(delta);

			fallRigidBody->getMotionState()->getWorldTransform(trans);
			std::cout << "sphere height: " << trans.getOrigin().getY() << std::endl;
            std::cout << "tick time: " << delta.count() << std::endl;

            auto error = glGetError();
            if(error) {
                console->error("OpenGL Error Code: {:d}", error);
            }
		});
	}

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

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
