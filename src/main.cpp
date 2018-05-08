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
#include "widget/GameMenu.h"
#include "widget/PlayerHud.h"
#include "widget/DebugTextHud.h"
#include "widget/HelpMenu.h"
#include "helper/ImageGenerator.h"

#include <kaguya/kaguya.hpp>
#include <spdlog/spdlog.h>


/*
 * main.cpp of Stage Fighter, this File contains the entry point of the executable and does bootstrap
 * all needed Classes and Settings from Files that the Game can create a Window and start ...
 */
int main(int argc, char *argv[]) {
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
                              config["window"]["refreshRate"]
    );

    /*
     * Prepare the fonts which are used by the Program (pre-loading) and load a charset so the font texture has not
     * to be rec
     */
    FontManager::load("Lato-Regular", "Lato-12", 12);
    FontManager::load("Lato-Regular", "Lato-24", 24);
    FontManager::load("Lato-Regular", "Lato-64", 64);
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


    // Generate Marble Texture ... doesn't look like marble :/
    //auto texData = ImageGenerator::marble(512, 512, glm::vec4(1.0, 1.0, 1.0, 1.0));
    //TextureManager::store("__gen_marble", texData, 512, 512);


    /*
     * Render a Loading Level Frame before loading the Level, this may take a while ...
     */
    auto loadingLabel = std::make_shared<Label>("Loading Level ...", FontManager::get("Lato-64"), 0, 0, 1.0, glm::vec3(0.0, 0.0, 0.0));
    loadingLabel->setPosition(window->getWidth()/2.0f-loadingLabel->getWidth()/2.0f, window->getHeight()/2.0f-32.0f);
    window->addWidget(loadingLabel);

    auto _curTick = std::chrono::high_resolution_clock::now();
    window->render(_curTick - lastTick);

    /*
     * Load and start Test level so we can do something
     */
    auto level = std::make_shared<Level>("../resources/level/test.lua");

    /*
     * Hide Mouse cursor and render another frame, to discard all the inputs
     */
    window->hideCursor();
    _curTick = std::chrono::high_resolution_clock::now();
    window->render(_curTick - lastTick);
    
    /*
     * Start the Level
     */
    level->start(window);
    window->removeWidget(loadingLabel);

    /*
     * For development purposes the Window can also handle direct camera movement aka "flying camera", we don't want
     * this in a game. Also the Player does directly control the camera ...
     * Disabling the Cursor is important so the Game does not loose the focus in window mode
     */
    window->processCameraMouseMovement(true);
    window->processCameraKeyMovement(false);


    /*
     * Create a Label which displays the FPS counter on the Screen, better than just spamming the console with
     * such a output
     */
    auto debugTextHud = std::make_shared<DebugTextHud>();

    /*
     * Initialize the GUI System "Nuklear" and the GameMenu which is displayed when pressing ESC
     */
    auto nuklear = std::make_shared<NuklearContext>(window);
    auto gameMenu = std::make_shared<GameMenu>(nuklear, level);
    auto helpMenu = std::make_shared<HelpMenu>(nuklear);
    nuklear->add(gameMenu);
    nuklear->add(helpMenu);

    gameMenu->gamma = config["window"]["gamma"];

    /*
     * Disable the nuklear GUI, so it's invisible and does not capture events
     * Add a Key Callback to the window so the GUI can be shown & hidden by the ESC key
     */
    nuklear->enabled = false;
    window->registerKeyCallback([nuklear, helpMenu, level, gameMenu, window](int key, int scancode, int action, int mods){
        if(key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
            if (gameMenu->isVisible()){
                nuklear->enabled = false;
                gameMenu->hide();
                level->resume();
            } else {
                level->pause();
                nuklear->enabled = true;
                gameMenu->show();
            }
        }

        if(key == GLFW_KEY_F1 && action == GLFW_RELEASE) {
            if (helpMenu->isVisible()) {
                nuklear->enabled = false;
                helpMenu->hide();
                level->resume();
            } else {
                level->pause();
                nuklear->enabled = true;
                helpMenu->show();
            }
        }

        if (key == GLFW_KEY_F12 && action == GLFW_RELEASE) {
            ShaderManager::recompileAll();
        }
    });


    /*
     * Add Nuklear and the Debug HUD to the Window so they are drawn each Frame
     */
    window->addWidget(nuklear);
    window->addWidget(debugTextHud);
    window->registerKeyCallback(debugTextHud->getKeyCallback());

    /*
     * ======= MAIN GAME LOOP =======
     * in lastTick count we want to save the last time we rendered a frame
     * and in frameSampleCount a accumulated number of frames is saved which is used
     * to limit the number of frames in which the fps llabelwill be redrawn ...
     */


    /*
     * While the window is open we want to render stuff ...
     * TODO: First delta might be way too small for some simulation stuff
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
        level->tick(delta);

        /*
         * Update Debug HUD information:
         */
        debugTextHud->update(delta);

        /*
         * Process nuklear frame
         * (Clear command buffer, capture events, ...)
         */
        nuklear->newFrame();

        /*
         * Finally draw all the Content which is registered to the Screen
         * (All Widgets and Object3Ds ->render(...) will be called)
         */
        window->render(delta);

        /*
         * glGetError() slows down rendering, since it forces the driver do flush everything so it can read back the
         * error code, normally it should be disabled if not developing and such ... but sometimes it might be nice
         * to just enable it and look at all those error codes (e.g. in VizLab)
         */
        if (openGlDbgFlag) {
            auto error = glGetError();
            if(error != GL_NO_ERROR) {
                console->error("OpenGL Error Code: {}", error);
            }
        }
	}

	/*
	 * After closing the Window we have to delete all the Resources and such ...
	 */
	level->destroy();
    window->showCursor();
    //TODO: remove keyCallback of DebugHUD

    debugTextHud.reset();
    nuklear.reset();
	delete window;

    ShaderManager::destroy();
    TextureManager::destroy();
    ModelManager::destroy();
    FontManager::destroy();

    /*
     * Terminate the GLFW Framework which was initialized in the Window class
     */
	glfwTerminate();

	// Exit with success state ...
	exit(EXIT_SUCCESS);
}
