//
// Created by Raphael on 28.03.2018.
//

#include "Level.h"

#include <glm/gtc/quaternion.hpp>

#include "../manager/FontManager.h"
#include "../manager/TextureManager.h"

#include "../object3d/Light.h"

Level::Level(const std::string &file) : Logger("Level"), world(std::make_shared<BulletUniverse>(btVector3(0,-9.81f,0))) {
    winLoseLabel = std::make_shared<Label>("", FontManager::get("Lato-64"), 0, 0, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
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
                .setConstructors<LuaStaticObject(std::string,std::string,LuaVec3,float,LuaVec4,kaguya::LuaTable,kaguya::LuaTable)>()
    );

    state["Entity"].setClass(kaguya::UserdataMetatable<LuaEntity>());
    state["ScriptedEntity"].setClass(
            kaguya::UserdataMetatable<LuaScriptedEntity, LuaEntity>()
                    .setConstructors<LuaScriptedEntity(std::string, int, LuaVec3&, float, LuaVec4&, std::string, float, LuaBtCollisionShape &, kaguya::LuaTable)>()
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

    state["Projectile"].setClass(
            kaguya::UserdataMetatable<LuaProjectile>()
                    .setConstructors<LuaProjectile(std::string, float, float, LuaBtCollisionShape&)>()
    );

    // Register Level to use global functions everywhere:
    state["Level"].setClass(
            kaguya::UserdataMetatable<Level>()
                    .addFunction("spawn", &Level::luaSpawnEntity)
                    .addFunction("getPlayerPos", &Level::luaGetPlayerPos)
    );
    state["level"] = this;

    // Finally load file
    state.dofile(file);

    // TODO: validate file ...

    auto curTick = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> delta =  curTick - past;
    logger->info("Finished loading file {} in {} ms", file, delta.count());
}

void Level::start(Window *window) {
    /*
     * Manipulation internal state since Camera and Window must not be present while loading the Level stuff
     * (lazy loading or pre-loading)
     */
    this->window = window;
    this->window->addWidget(this->loadingStatusLabel);
    auto past = std::chrono::high_resolution_clock::now();

    this->player = std::make_shared<Player>(window->getScene()->getCamera(), window, world);
    this->playerWitcheryPointer = std::shared_ptr<Object3DAbstract>(dynamic_cast<Object3DAbstract *>(this->player.get()));

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

    player->setPosition(pPosition->pos, glm::quat(0, 0, 0, 1));

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
    for (auto &obj : state["objects"].map<int, LuaStaticObject *>()) {
        this->setLoadingStatus("objects", c++, all);
        this->statics.push_back(obj.second->toModel());

        auto col = obj.second->collisions();
        this->bullet.insert(bullet.end(), col.begin(), col.end());
    }

    c=1;
    all = static_cast<int>(this->bullet.size());
    for (auto &bulletObj : this->bullet) {
        this->setLoadingStatus("bullet physics", c++, all);
        world->addRigidBody(bulletObj->getRigidBody());
    }

    // TODO: InstancedProjectile:
    c=1;
    all = static_cast<int>( state["projectiles"].size());
    for(auto &projectile : state["projectiles"].map<std::string, LuaProjectile *>()) {
        this->setLoadingStatus("projectiles", c++, all);

        logger->info("Loading projectile: {}", projectile.first);
    }

    projectiles[std::string("bullet")] = std::make_shared<InstancedProjectile>(
            0.0f,  ModelManager::load("bullet"), ShaderManager::load("standard-instanced"),
            btVector3(0.174505f/2,0.174505f/2,0.286695f/2), 0.00001, world
    );

    this->logger->info("Loaded {} entities", entities.size());
    this->logger->info("Loaded {} objects", statics.size());
    this->logger->info("Loaded {} collision primitives", bullet.size());
    this->logger->info("Loaded {} light sources", lights.size());

    //this->logger->info("Player.add={}", (void*)player.get());

    this->window->removeWidget(this->loadingStatusLabel);

    auto curTick = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> delta = curTick - past;
    logger->info("Level loaded in {} ms", delta.count());

    this->show();

    player->lookAt(pLookAt->pos);
}

void Level::destroy() {
    this->hide();

    this->entities.clear();
    this->statics.clear();
    window->removeKeyPollingCallback(playerInputCallbackID);
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

    for (auto &pSys : this->projectiles)
        pSys.second->think(delta);

    int enemyH = 0;
    for (auto &entity : this->entities) {
        if (entity->mustBeKilled)
            enemyH += entity->getHealth();

        entity->think(this, delta);
    }

    player->think(delta);
    player->computeEnemyInView(entities);

    if (enemyH <= 0) {
        pause();
        levelState = WON;
        this->setLabel("You won!");
        logger->info("You won!");
    }

    if (player->getHealth() < 0) {
        pause();
        levelState = LOST;
        this->setLabel("You lost");
        logger->info("You lost!");
    }
}

void Level::resetEnvironment() {
    this->hide();

    // Clear generated Level
    this->entities.clear();
    this->oldEntities.clear();
    this->bullet.clear();
    this->lights.clear();

    this->window->removeKeyPollingCallback(playerInputCallbackID);

    // Re-Load from Lua Environment
    this->start(this->window);
}

void Level::hide() {
    pause();
    for (auto &entity : this->entities) this->window->getScene()->removeObject(entity);
    for (auto &obj    : this->statics ) this->window->getScene()->removeObject(obj);
    for (auto &light  : this->lights  ) this->window->getScene()->removeLight(light);

    this->window->removeWidget(player->getHud());
    this->window->removeWidget(winLoseLabel);
    this->window->getScene()->removeObject(this->playerWitcheryPointer);
    for (auto &projectile : this->projectiles) {
        const std::shared_ptr<Object3DAbstract> ptr = std::dynamic_pointer_cast<Object3DAbstract>(projectile.second);
        this->window->getScene()->removeObject(ptr);
        this->window->getScene()->removeParticleSystem(projectile.second);
    }
}

void Level::show() {
    resume();
    for (auto &entity : this->entities) this->window->getScene()->addObject(entity);
    for (auto &obj    : this->statics ) this->window->getScene()->addObject(obj);
    for (auto &light  : this->lights  ) this->window->getScene()->addLight(light);

    this->window->addWidget(player->getHud());
    if (levelState != PLAYING) {
        this->window->addWidget(winLoseLabel);
    }

    this->window->getScene()->addObject(this->playerWitcheryPointer);
    for (auto &projectile : this->projectiles) {
        const std::shared_ptr<Object3DAbstract> ptr = std::dynamic_pointer_cast<Object3DAbstract>(projectile.second);
        this->window->getScene()->addObject(projectile.second);
        this->window->getScene()->addParticleSystem(projectile.second);
    }
}

void Level::pause() {
    player->disable();
    levelState = PAUSED;
    setLabel("Paused");
}

void Level::resume() {
    player->enable();
    levelState = PLAYING;
    window->removeWidget(winLoseLabel);
}

void Level::spawn(std::shared_ptr<Entity> entity) {
    this->newEntities.push_back(entity);
    this->window->getScene()->addObject(entity);


    if (entity->getEntityKind() == BulletObject::BULLET) {
        auto *o = dynamic_cast<BulletEntity *>(entity.get());
        this->window->getScene()->addParticleSystem(o->getSmoke());
    }
}

void Level::despawn(Entity *entity) {
    this->oldEntities.push_back(entity);

    if (entity->getEntityKind() == BulletObject::BULLET) {
        auto *o = dynamic_cast<BulletEntity *>(entity);
        this->window->getScene()->removeParticleSystem(o->getSmoke());
    }
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
    this->player->disable();
    this->hide();
    this->window->removeKeyPollingCallback(playerInputCallbackID);
}

void Level::luaSpawnEntity(const LuaProjectile &projectile, const LuaVec3 &spawn, const LuaVec3 &target) {
    this->projectiles["bullet"]->spawn(spawn.toVector3(), target.toVector3(), 5.0f);
    //this->spawn(projectile.toBulletEntity(spawn, target, world));
}

LuaVec3 Level::luaGetPlayerPos() {
    auto p = this->player->getPosition();
    return LuaVec3(p.x, p.y, p.z);
}
