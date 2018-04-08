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
#include "widget/NuklearCtx.h"

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

    // Create Window and Camera system:
    Camera camera(glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                  -90.0f, 0.0f,
                  config["camera"]["fov"], config["window"]["width"], config["window"]["height"],
                  0.01f, 1000.0f
    );
	auto *window = new Window(camera, config["window"]["width"], config["window"]["height"], "Stage Fighter",
                              config["window"]["fullscreen"], config["window"]["refreshRate"]
    );

	// Configure Window Stuff:
    window->setVSync(config["window"]["vsync"]);
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
    auto t = std::string("Stage Figther");
    auto text = std::make_shared<Label>(t, FontManager::load("Lato-Regular"), 0.0f, 25.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    text->setPosition(window->getWidth()/2.0f-text->getWidth()/2.0f, 52.0f);
    window->addWidget(text);

    // Nuklear Test:
    auto n = std::make_shared<NuklearContext>(window);
    window->addWidget(n);

    struct nk_colorf bg;
    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;

    // Enter main game Loop:
    auto lastTick = std::chrono::high_resolution_clock::now();
    double frameSampleCount = 0.0;
    bool openGlDbgFlag = config["debug"]["opengl"];

    n->enabled = false;
    window->registerKeyCallback([n, window](int key, int scancode, int action, int mods){
        if(key == GLFW_KEY_0 && action == GLFW_PRESS) {
            n->enabled = !n->enabled;

            if (n->enabled) {
                window->showCurosor();
            } else {
                window->hideCursor();
            }
        }
    });

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

        n->newFrame();
        nk_begin(n->context(), "Test", nk_rect(50, 50, 230, 250),
                 NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE);
        {
            auto ctx = n->context();

            enum {
                EASY, HARD
            };
            static int op = EASY;
            static int property = 20;
            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");

            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx), 400))) {
                nk_layout_row_dynamic(ctx, 120, 1);
                bg = nk_color_picker(ctx, bg, NK_RGBA);
                nk_layout_row_dynamic(ctx, 25, 1);
                bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
                bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
                bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
                bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f, 0.005f);
                nk_combo_end(ctx);
            }
        }
        nk_end(n->context());

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
