//
// Created by Raphael on 28.03.2018.
//

#include "Level.h"

#include <memory>
#include <glm/gtc/quaternion.hpp>
#include <stb_perlin.h>

#include "../manager/FontManager.h"
#include "../manager/TextureManager.h"

#include "../object3d/Light.h"
#include "../manager/MenuManager.h"

#include "../widget/GameMenu.h"
#include "../GlobalGameState.h"
#include "../manager/AudioManager.h"

Level::Level(const std::string &file) : Logger("Level"), world(std::make_shared<BulletUniverse>(btVector3(0,-9.81f,0))) {
    winLoseLabel = std::make_shared<Label>("", FontManager::get("Metamorphous-64"), 0, 0, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    loadingStatusLabel = std::make_shared<Label>("", FontManager::get("Lato-12"), 0, 0, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));

    logger->info("Loading file {}", file);
    auto past = std::chrono::high_resolution_clock::now();

    // Setup Lua Environment:
    state["vec3"].setClass(
            kaguya::UserdataMetatable<LuaVec3>()
                    .setConstructors<LuaVec3(double, double, double)>()
                    .addFunction("x", &LuaVec3::x)
                    .addFunction("y", &LuaVec3::y)
                    .addFunction("z", &LuaVec3::z)
                    .addFunction("distance", &LuaVec3::distance)
    );
    state["vec4"].setClass(
            kaguya::UserdataMetatable<LuaVec4>()
                    .setConstructors<LuaVec4(double, double, double, double)>()
                    .addFunction("x", &LuaVec4::x)
                    .addFunction("y", &LuaVec4::y)
                    .addFunction("z", &LuaVec4::z)
                    .addFunction("w", &LuaVec4::w)
    );

    state["StaticObject"].setClass(
            kaguya::UserdataMetatable<LuaStaticObject>()
                .setConstructors<LuaStaticObject(std::string,kaguya::LuaTable,LuaVec3,float,LuaVec4,kaguya::LuaTable,kaguya::LuaTable)>()
    );

    state["Entity"].setClass(kaguya::UserdataMetatable<LuaEntity>());
    state["ScriptedEntity"].setClass(
            kaguya::UserdataMetatable<LuaScriptedEntity, LuaEntity>()
                    .setConstructors<LuaScriptedEntity(std::string, kaguya::LuaTable,int, LuaVec3&, float, LuaVec4&, std::string, float, LuaBtBox &, kaguya::LuaTable)>()
    );
    state["ScriptedObject"].setClass(
            kaguya::UserdataMetatable<LuaScriptedObject>()
                    .setConstructors<LuaScriptedObject(std::string, kaguya::LuaTable, LuaVec3&, float, LuaVec4&,
                                                       LuaBtBox&, kaguya::LuaTable, int, kaguya::LuaTable)>()
    );


    state["btCollisionShape"].setClass(kaguya::UserdataMetatable<LuaBtCollisionShape>());
    state["SphereShape"].setClass(
            kaguya::UserdataMetatable<LuaBtSphere, LuaBtCollisionShape>()
                    .setConstructors<LuaBtSphere(LuaVec3&, LuaVec4&, double, double)>()
    );
    state["BoxShape"].setClass(
            kaguya::UserdataMetatable<LuaBtBox, LuaBtCollisionShape>()
                    .setConstructors<LuaBtBox(LuaVec3&, LuaVec4&, double, LuaVec3&)>()
    );

    state["PointLight"].setClass(
            kaguya::UserdataMetatable<LuaLight>()
                    .setConstructors<LuaLight(LuaVec3&, LuaVec3&, LuaVec3&, LuaVec3&, float)>()
    );

    state["ParticleSystem"].setClass(
            kaguya::UserdataMetatable<LuaScriptedParticleSystem>()
                    .setConstructors<LuaScriptedParticleSystem(float, kaguya::LuaTable, std::string, kaguya::LuaTable)>()
    );

    state["Projectile"].setClass(
            kaguya::UserdataMetatable<LuaProjectile>()
                    .setConstructors<LuaProjectile(std::string, kaguya::LuaTable, float, float, float, LuaVec3&, LuaScriptedParticleSystem&)>()
    );

    state["Trigger"].setClass(
            kaguya::UserdataMetatable<LuaTrigger>()
                    .setConstructors<LuaTrigger(LuaVec3&, float, kaguya::LuaTable)>()
    );

    // Register Level to use global functions everywhere:
    state["Level"].setClass(
            kaguya::UserdataMetatable<Level>()
                    .addFunction("spawnProjectile", &Level::luaSpawnProjectile)
                    .addFunction("getPlayerPos", &Level::luaGetPlayerPos)
                    .addFunction("spawnEntity", &Level::luaSpawnEnitity)
                    .addFunction("showTextbox", &Level::luaShowTextbox)
                    .addFunction("hideTextbox", &Level::luaHideTextbox)
                    .addFunction("play3DSound", &Level::luaPlay3DSound)
                    .addFunction("play2DSound", &Level::luaPlay2DSound)
    );
    state["level"] = this;

    // Register some utility stuff like noise functions:
    state["noise"] = kaguya::NewTable();
    state["noise"]["stb_perlin_ridge_noise3"] = &stb_perlin_ridge_noise3;

    state["Sound"].setClass(
            kaguya::UserdataMetatable<LuaSound>()
                    .addFunction("stop", &LuaSound::stop)
                    .addFunction("destroy", &LuaSound::destroy)
                    .addFunction("setVolume", &LuaSound::setVolume)
                    .addFunction("setPosition", &LuaSound::setPosition)
                    .addFunction("resume", &LuaSound::resume)
                    .addFunction("setLoop", &LuaSound::loop)
    );


    // Finally load file
    state.dofile(file);

    auto skyboxTex = std::make_shared<CubemapTexture>(state["skybox"]["file"], state["skybox"]["ext"]);
    this->skybox = std::make_shared<Skybox>(skyboxTex, std::vector<std::shared_ptr<Shader>> {ShaderManager::load("skybox")});

    // Display how long it took to load / parse all the LUA files
    auto curTick = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> delta =  curTick - past;
    logger->info("Finished loading file {} in {} ms", file, delta.count());

    // Create Game Menu
    this->gameMenu = std::make_shared<GameMenu>(MenuManager::getNuklearContext(), this);
    this->gameMenuCallback = MenuManager::getWindow()->registerKeyCallback([this](int key, int UNUSED(scancode), int action, int UNUSED(mods)){
       if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
           MenuManager::toggleMenu(MenuManager::View::GAME_MENU, true);
           MenuManager::getNuklearContext()->disableInput = false;
       }
    });

    MenuManager::getMenus()[MenuManager::View::GAME_MENU] = gameMenu;
}

void Level::start(Window *window) {
    // Set global MenuManager state:
    MenuManager::transitionIntoLevel(this);

    /*
     * Manipulation internal state since Camera and Window must not be present while loading the Level stuff
     * (lazy loading or pre-loading)
     */
    this->window = window;
    this->window->addWidget(this->loadingStatusLabel);
    this->window->getScene()->setSkybox(skybox);
    auto past = std::chrono::high_resolution_clock::now();

    this->player = std::make_shared<Player>(window->getScene()->getCamera(), window, world);
    //this->playerWitcheryPointer = std::shared_ptr<Object3DAbstract>(dynamic_cast<Object3DAbstract *>(this->player.get()));

    /*
     * Enable the Debugging Stuff of bullet if debugging is enabled
     */
    if (this->world->isDebugging())
        window->getScene()->bulletDebugDrawer = world->getDebugDrawer();

    /*
     * Initialize Player in the Level:
     */
    player->entitySpeed = state["player"]["speed"];
    const LuaVec3 *pPosition = state["player"]["position"];
    const LuaVec3 *pLookAt = state["player"]["lookAt"];

    player->setPosition(pPosition->vec3, glm::quat(0, 0, 0, 1));

    playerInputCallbackID = window->registerKeyPollingCallback(player->getKeyboardCallback());

    int c=1;
    int all = static_cast<int>(state["entities"].size());
    for (auto &entity : state["entities"].map<int, LuaEntity *>()) {
        this->setLoadingStatus("entities", c++, all);
        this->entities.push_back(entity.second->toEntity3D(this->world));
    }

    c=1;
    all = static_cast<int>(state["lights"].size());
    for (auto &light : state["lights"].map<int, LuaLight*>()) {
        this->setLoadingStatus("lights", c++, all);
        this->lights.push_back(light.second->toLight());
    }

    c=1;
    all = static_cast<int>(state["objects"].size());
    if (statics.empty())
        for (auto &obj : state["objects"].map<int, LuaStaticObject *>()) {
            this->setLoadingStatus("objects", c++, all);
            this->statics.push_back(obj.second->toModel());

            auto col = obj.second->collisions();
            this->bullet.insert(bullet.end(), col.begin(), col.end());
        }

    c=1;
    all = static_cast<int>(state["scriptedObjects"].size());
    for (auto &obj : state["scriptedObjects"].map<int, LuaScriptedObject *>()) {
        this->setLoadingStatus("scripted objects", c++, all);
        sObjects.push_back(obj.second->toScriptedObject(world));
    }

    c=1;
    all = static_cast<int>(this->bullet.size());
    for (auto &bulletObj : this->bullet) {
        this->setLoadingStatus("bullet physics", c++, all);
        world->addRigidBody(bulletObj->getRigidBody());
    }

    c=1;
    all = static_cast<int>( state["projectiles"].size());
    for(auto &projectile : state["projectiles"].map<int, LuaProjectile *>()) {
        this->setLoadingStatus("projectiles", c++, all);
        this->projectiles.push_back(projectile.second->toProjectile(world));
    }

    c=1;
    all = static_cast<int>( state["triggers"].size());
    for(auto &projectile : state["triggers"].map<int, LuaTrigger *>()) {
        this->setLoadingStatus("triggers", c++, all);
        this->triggers.push_back(projectile.second->toTrigger());
    }

    c=1;
    all = static_cast<int>( state["particlesystem"].size());
    if (particleSystem.empty())
        for(auto &t : state["particlesystem"].map<int, kaguya::LuaTable>()) {
            this->setLoadingStatus("particles", c++, all);
            const auto position = t.second["position"].get<LuaVec3>();
            const auto ps = t.second["particle"].get<LuaScriptedParticleSystem>();

            this->particleSystem.push_back(ps.toParticleSystem(position.vec3, t.second["number"], true));
        }

    this->logger->info("Loaded {} entities", entities.size());
    this->logger->info("Loaded {} objects", statics.size());
    this->logger->info("Loaded {} collision primitives", bullet.size());
    this->logger->info("Loaded {} light sources", lights.size());
    this->logger->info("Loaded {} projectiles", projectiles.size());
    this->logger->info("Loaded {} scripted objects", sObjects.size());
    this->logger->info("Loaded {} triggers", triggers.size());
    this->logger->info("Loaded {} particles", particleSystem.size());

    this->window->removeWidget(this->loadingStatusLabel);

    auto curTick = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> delta = curTick - past;
    logger->info("Level loaded in {} ms", delta.count());

    this->levelState = PLAYING;
    this->show();
    player->lookAt(pLookAt->vec3);
}

void Level::tick(std::chrono::duration<double, std::milli> delta) {
    if (levelState != PLAYING)
        return;

    /*
     * Simulate all the Bullet stuff, Collisions and so on will happen here
     * If debugging is enabled then we also want to recompute the Bullet debugging
     * context (all these lines ...)
     */
    world->simulate(delta);
    if (world->isDebugging())
        world->drawDebug();

    // Add and Remove Entities:
    {
        for (auto &entity : this->oldEntities)
            this->entities.erase(
                    std::remove_if(
                            this->entities.begin(),
                            this->entities.end(),
                            [entity, this](std::shared_ptr<Entity> current) -> bool {
                                bool r = current.get() == entity;
                                if (r)
                                    window->getScene()->removeObject(current);

                                return r;
                            }
                    ),
                    this->entities.end()
            );

        for(auto &n : newEntities)
            entities.push_back(n);


        newEntities.clear();
        oldEntities.clear();
    }

    /*
     * Update all the Scripted and non scripted
     * Objects and Entities in the world
     */
    for (auto &pSys : this->projectiles)
        pSys->think(delta);

    for (auto &so : this->sObjects)
        so->think(this, delta);

    int enemyH = 0;
    for (auto &entity : this->entities) {
        if (entity->mustBeKilled)
            enemyH += entity->getHealth();

        entity->think(this, delta);
    }

    // Update player stuff
    player->think(delta);
    player->computeEnemyInView(entities);


    // Process triggers
    for (auto &trigger : this->triggers) {
        trigger->update(player->getPosition());
    }

    // Finally check for the win condition
    if (enemyH <= 0) {
        pause();
        levelState = WON;
        this->setLabel("Victory!");
        MenuManager::showMenu(MenuManager::LEVEL_FINISHED_MENU);
        state["victory"]();
    }

    if (player->getHealth() <= 0 || player->getPosition().y < state["deathHeight"].get<float>()) {
        pause();
        levelState = LOST;
        this->setLabel("Game Over!");
        MenuManager::showMenu(MenuManager::LEVEL_FINISHED_MENU);
        state["lost"]();
    }
}

void Level::resetEnvironment() {
    this->levelState = Level::PAUSED;
    this->hide();

    // Clear generated Level
    this->entities.clear();         // Moved, must be re-created
    this->oldEntities.clear();      // tmp array
    this->newEntities.clear();      // tmp array
    this->lights.clear();           // might have been modified
    this->sObjects.clear();         // scripted objects, don't trust that they behave the same
    this->projectiles.clear();      // instanced projectiles (warning: contains black magic)
    this->triggers.clear();         // nobody knows their internal state
    // statics & bullet objects from them are re-used

    // Clear lights in Window
    window->getScene()->getLights().clear();
    this->window->removeKeyPollingCallback(playerInputCallbackID);

    // Remove static bullet stuff
    for (auto &b : this->bullet)
        world->removeRigidBody(b->getRigidBody());

    // Re-Load from Lua Environment
    this->start(this->window);
}

void Level::hide() {
    state["hide"]();

    pause(false);
    for (auto &entity : this->entities ) this->window->getScene()->removeObject(entity);
    for (auto &obj    : this->statics  ) this->window->getScene()->removeObject(obj);
    for (auto &obj    : this->sObjects ) { this->window->getScene()->removeObject(obj); obj->hide(window->getScene().get()); }
    for (auto &light  : this->lights   ) this->window->getScene()->removeLight(light);
    for (auto &ps     : this->particleSystem) this->window->getScene()->removeParticleSystem(ps);

    this->window->removeWidget(player->getHud());
    this->window->removeWidget(winLoseLabel);
    this->window->getScene()->removeObject(this->player);
    this->window->getScene()->removeSkybox();
    for (auto &projectile : this->projectiles) {
        const std::shared_ptr<Object3DAbstract> ptr = std::dynamic_pointer_cast<Object3DAbstract>(projectile);
        this->window->getScene()->removeObject(ptr);
        this->window->getScene()->removeParticleSystem(projectile);
    }
}

void Level::show() {
    state["show"]();

    resume();
    for (auto &entity : this->entities ) this->window->getScene()->addObject(entity);
    for (auto &obj    : this->statics  ) this->window->getScene()->addObject(obj);
    for (auto &obj    : this->sObjects ) { this->window->getScene()->addObject(obj); obj->show(window->getScene().get()); }
    for (auto &light  : this->lights   ) this->window->getScene()->addLight(light);
    for (auto &ps     : this->particleSystem) this->window->getScene()->addParticleSystem(ps);

    this->window->addWidget(player->getHud());
    if (levelState != PLAYING) {
        this->window->addWidget(winLoseLabel);
    }

    this->window->getScene()->addObject(this->player);
    this->window->getScene()->setSkybox(this->skybox);
    for (auto &projectile : this->projectiles) {
        const std::shared_ptr<Object3DAbstract> ptr = std::dynamic_pointer_cast<Object3DAbstract>(projectile);
        this->window->getScene()->addObject(projectile);
        this->window->getScene()->addParticleSystem(projectile);
    }
}

void Level::pause(bool showStatus) {
    player->disable();
    player->setCrosshairState(false);
    levelState = PAUSED;
    if (showStatus) setLabel("Paused");
}

void Level::resume() {
    player->enable();
    player->setCrosshairState(true);
    levelState = PLAYING;
    window->removeWidget(winLoseLabel);
}

void Level::setLabel(const std::string text) {
    winLoseLabel->setText(text);
    winLoseLabel->setPosition(
            window->getWidth()  / 2.0f - winLoseLabel->getWidth() / 2.0f,
            window->getHeight() / 4.0f - 64.0f/2.0f
    );

    if (levelState != PLAYING)
        window->addWidget(winLoseLabel);
}

void Level::setLoadingStatus(std::string thing, int cur, int max) {
    char buffer[256];
    snprintf(buffer, 256, "Loading %s (%d / %d)", thing.c_str(), cur, max);

    loadingStatusLabel->setText(buffer);
    loadingStatusLabel->setPosition(
                window->getWidth() / 2.0f - loadingStatusLabel->getWidth() / 2.0f,
                window->getHeight() - 64.0f
            );

    auto zDeltaT = std::chrono::duration<double, std::milli>::zero();
    this->window->render(zDeltaT);
}

Level::~Level() {
    this->window->removeKeyPollingCallback(playerInputCallbackID);
    this->window->removeKeyCallback(gameMenuCallback);
    for (auto &b : this->bullet)
        world->removeRigidBody(b->getRigidBody());

    this->player->disable();
    this->hide();
}

void Level::luaSpawnProjectile(const int projectile, const LuaVec3 &spawn, const LuaVec3 &target) {
    this->projectiles[projectile-1]->spawn(spawn.toVector3(), target.toVector3(), 5.0f);
}

LuaVec3 Level::luaGetPlayerPos() {
    auto p = this->player->getPosition();
    return LuaVec3(p.x, p.y, p.z);
}

void Level::despawn(Entity *entity) {
    this->oldEntities.push_back(entity);
}

void Level::luaSpawnEnitity(const LuaEntity &entity, const LuaVec3 &spawn) {
    auto nEntity = entity.toEntity3D(world);
    nEntity->setPosition(spawn.vec3, glm::quat(0,0,0,1));

    this->newEntities.push_back(nEntity);
}

void Level::luaShowTextbox(const std::string &text) {
    std::dynamic_pointer_cast<TextBoxWindow>(MenuManager::getMenus()[MenuManager::LEVEL_TEXT_BOX])->setText(text);
    MenuManager::showMenu(MenuManager::LEVEL_TEXT_BOX, false);

    // Just for debugging
    this->logger->info("Show Text: {}", text);
}

void Level::luaHideTextbox() {
    MenuManager::hideMenu(false);
}

LuaSound Level::luaPlay3DSound(const std::string &file, const LuaVec3 &position, bool looped) {
    return LuaSound(
            AudioManager::audioEngine->play3D(file.c_str(),
                                             irrklang::vec3df(static_cast<irrklang::ik_f32>(position.x()),
                                                              static_cast<irrklang::ik_f32>(position.y()),
                                                              static_cast<irrklang::ik_f32>(position.z())),
                                             looped, false, true
        )
    );
}

LuaSound Level::luaPlay2DSound(const std::string &file, bool looped) {
    return LuaSound(AudioManager::audioEngine->play2D(file.c_str(), looped, false, true));
}
