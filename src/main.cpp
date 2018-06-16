#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <chrono>
#include <memory>
#include <thread>
#include <future>

#include <btBulletDynamicsCommon.h>
#include <kaguya/kaguya.hpp>
#include <spdlog/spdlog.h>

#include "Window.h"
#include "BulletUniverse.h"
#include "CubemapTexture.h"

#include "manager/ShaderManager.h"
#include "manager/TextureManager.h"
#include "manager/ModelManager.h"
#include "manager/FontManager.h"

#include "object3d/Model3DObject.h"
#include "object3d/Skybox.h"
#include "object3d/ParticleSystem.h"

#include "controller/CameraController.h"

#include "level/Level.h"

#include "widget/Label.h"
#include "widget/NuklearCtx.h"
#include "widget/GameMenu.h"
#include "widget/PlayerHud.h"
#include "widget/DebugTextHud.h"
#include "widget/HelpMenu.h"

#include "helper/ImageGenerator.h"
#include "helper/CompilerMacros.h"
#include "manager/MenuManager.h"
#include "LaunchScreen.h"
#include "GlobalGameState.h"

/*
 * main.cpp of Stage Fighter, this File contains the entry point of the executable and does bootstrap
 * all needed Classes and Settings from Files that the Game can create a Window and start ...
 */
int main(int UNUSED(argc), char** UNUSED(argv)) {
    /*
     * Setup async logger with a buffer size of 8192 Messages, the default logger will be named
     * "console" and can be received by spdlog::get("console") everywhere in the program
     */
    spdlog::set_async_mode(8192);
    auto console = spdlog::stdout_color_mt("console");

    /*
     * Initialize GLFW Lib outside of Window class (so we can theoretically open multiple windows)
     */
    if(!glfwInit()) {
        console->error("Failed to initialize GLFW!");
        exit(EXIT_FAILURE);
    }

    /*
     * Creating a global Lua sate for configuration settings, these settings are read from a file
     * ../config.lua and saved in the state for the runtime of the program
     */
    console->info("Loading ../config.lua");
	kaguya::State config;

	//TODO: Try + typecheck
	config.dofile("../config.lua");

    bool openGlDbgFlag = config["debug"]["opengl"];
    BulletUniverse::debuggingFlag = config["debug"]["bullet"];

    if (openGlDbgFlag)                  console->info("Enabling OpenGL debugging");
    if (BulletUniverse::debuggingFlag)  console->info("Enabling Bullet DebugContext");

    /*
     * Initialize the "Manager" infrastructure which do load the resource once when requested and keep them in
     * memory for faster access, a root path is set so only the name or subdirectories must be specified while
     * loading resources with ::load(....)
     */
	console->info("Shader root is ../resources/shader");
    ShaderManager::build("../resources/shader/");
    TextureManager::build("../resources/texture/");
    FontManager::build("../resources/fonts/");
    ModelManager::build("../resources/");

    /*
     * At this state a default Camera with settings from the configuration file is created and a window with the
     * given settings:
     * (The window class will ignore invalid settings, but parsing will fail if the type is not correct)
     */
    auto lastTick = std::chrono::high_resolution_clock::now();
    Camera camera(glm::vec3(0.0f, 1.0f, 3.0f),  // Position in the OpenGL Space
                  glm::vec3(0.0f, 1.0f, 0.0f),  // Up Vector
                  -90.0f, 0.0f,                 // Yaw & Pitch of the Camera in degrees
                  config["camera"]["fov"],
                  config["window"]["width"],
                  config["window"]["height"],
                  0.01f, 1000.0f                // zNear & zFar for clipping stuff
    );
	auto *window = new Window(camera,
                              config["window"]["width"],
                              config["window"]["height"],
                              "Stage Fighter",
                              config["window"]["fullscreen"],
                              config["window"]["refreshRate"],
                              config["debug"]["opengl"]
    );

    /*
     * Prepare the fonts which are used by the Program (pre-loading) and load a charset so the font texture has not
     * to be rec
     */
    FontManager::load("Lato-Regular", "Lato-12", 12);
    FontManager::load("Lato-Regular", "Lato-24", 24);
    FontManager::load("Lato-Regular", "Lato-64", 64);
    FontManager::load("Metamorphous-Regular", "Metamorphous-64", 64);
    FontManager::load("Metamorphous-Regular", "Metamorphous-72", 72);
    FontManager::preloadCharset(
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "1234567890"
            "!\"§$%&/\\[]()=?;:_,.-+|<>#~'*ß{}^°"
            " "
            );

	/*
	 * Additionally we want to set other Window properties like vsync or gamma if we run in Fullscreen mode
	 */
    window->setVSync(config["window"]["vsync"]);
    if (config["window"]["useShaderGamma"].get<bool>()) {
        window->setScreenGamma(config["window"]["gamma"]);
    } else {
        if (window->canSetGamma()) window->setGamma(config["window"]["gamma"]);
        else if (config["window"]["gamma"].get<float>() != 1.0) {
            console->critical("Can not set gamma value in windowed mode!");
            console->info("Function setScreenGamma will be used!");
            window->setScreenGamma(config["window"]["gamma"]);
        }
    }

    /*
     * Configure Global Bullet debugging Mode: (Drawing Bullet primitives and such onto the Screen)
     */
    BulletUniverse::debuggingFlag = config["debug"]["bullet"];

    /* TODO:
     *  Normally we should show a Loading Screen and pre-load all resources which are needed directly after Window creation
     *  for displaying stuff and then showing the Main menu of the Game with a level selection ...
     */
    MenuManager::init(window);

    auto loadingLabel = std::make_shared<Label>("Generating resources ...", FontManager::get("Metamorphous-72"), 0, 0, 1.0, glm::vec3(0.9, 0.9, 0.9));
    loadingLabel->setPosition(window->getWidth()/2.0f-loadingLabel->getWidth()/2.0f, window->getHeight()/2.0f-32.0f);
    window->addWidget(loadingLabel);


    // Generate Marble Texture ...
    // Compute all the Resources in another thread:
    auto asyncTexData = std::async(std::launch::async, [] {
        return ImageGenerator::marble(256, 256, glm::vec4(180.0/255, 180.0/255, 180.0/255, 1.0));
    });

    // Async resource loading & events from window polling so it doesn't hang
    while(asyncTexData.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) {
        auto _curTick = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> deltaT = _curTick - lastTick;

        window->render(deltaT);
        lastTick = _curTick;
    }

    auto texData = asyncTexData.get();
    TextureManager::store("__gen_marble", texData, 256, 256);

    /*
     * "Steal" the focus from another Window if it has been taken ...
     */
    window->requestFocus();
    window->removeWidget(loadingLabel);

    /*
     *  Create main screen and process debugging options to jump
     *  directly into a level if given
     */
    auto main_screen = std::make_shared<LaunchScreen>(window);
    if (config["debug"]["loadIntoLevel"].get<bool>()) {
        auto levelFile = config["debug"]["levelFile"].get<std::string>();
        console->info("Loading directly into Level: {}", levelFile);

        GlobalGameState::level = std::make_shared<Level>(std::string("../resources/level/") + levelFile);
        GlobalGameState::state = GlobalGameState::LEVEL_LOADING_FINISHED;
    } else {
        main_screen->show();
    }

    /*
     * For development purposes the Window can also handle direct camera movement aka "flying camera", we don't want
     * this in a game. Also the Player does directly control the camera ...
     * Disabling the Cursor is important so the Game does not loose the focus in window mode
     */
    //window->processCameraMouseMovement(true);
    //window->processCameraKeyMovement(false);

    // Toggle Free flying camera:
    //level->getPlayer()->disable();
    //window->getScene()->getCamera().enableUpdate = true;
    //window->getScene()->getCamera().keySensitivity = 0.05;
    //window->processCameraKeyMovement(true);

    // Does not work
    window->getScene()->frustumCulling = false;


    /*
     * ======= MAIN GAME LOOP =======
     */

    /*
     * While the window is open we want to render stuff ...
     */
	while (window->isOpen()) {
	    /*
	     * Read current time and compute the delta of last and current timestamp
	     */
		auto curTick = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> delta =  curTick - lastTick;
		lastTick = curTick;

        /*
         * Process level logic and so on
         */
        switch (GlobalGameState::state) {
            case GlobalGameState::IN_MENU:break;

            case GlobalGameState::LEVEL_LOADING_FINISHED:
                main_screen->hide();
                window->processCameraMouseMovement(true);
                window->processCameraKeyMovement(false);
                GlobalGameState::level->start(window);
                GlobalGameState::state = GlobalGameState::IN_LEVEL;
                break;

            case GlobalGameState::IN_LEVEL:
                GlobalGameState::level->tick(delta);
                break;

            case GlobalGameState::JUMP_TO_MAIN_SCREEN:
                main_screen->show();
                GlobalGameState::state = GlobalGameState::IN_MENU;
                break;

            case GlobalGameState::AFTER_LEVEL:
                main_screen->show();
                GlobalGameState::level.reset();
                GlobalGameState::state = GlobalGameState::IN_MENU;
                break;

            case GlobalGameState::LEVEL_PLAY_AGAIN:
                GlobalGameState::level->resetEnvironment();
                window->hideCursor();
                GlobalGameState::state = GlobalGameState::IN_LEVEL;
                break;
        }

        /*
         * Update the menus of the MenuManger
         */
        MenuManager::update(delta);
        /*
         * Finally draw all the Content which is registered to the Screen
         * (All Widgets and Object3Ds ->render(...) will be called)
         */
        window->render(delta);
	}

	/*
	 * After closing the Window we have to delete all the Resources and such ...
	 */
	//level->destroy();
    window->showCursor();

    // Clean up all resources which depend on a OpenGL Context:
    if (GlobalGameState::level.use_count() > 0 && GlobalGameState::level != nullptr) {
        GlobalGameState::level.reset();
    }

    MenuManager::destroy();

    // Delete the OpenGL Context + Window
	delete window;

	// Delete all other resources ...
    ShaderManager::destroy();
    TextureManager::destroy();
    ModelManager::destroy();
    FontManager::destroy();

    /*
     * Terminate the GLFW Framework which was initialized in the Window class
     */
	glfwTerminate();

	// Exit with success state ...
    // (All other destructors will be called in *some* order -> might break something)
	exit(EXIT_SUCCESS);
}
