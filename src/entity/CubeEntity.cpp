//
// Created by Raphael on 17.03.2018.
//

#include "CubeEntity.h"

CubeEntity::CubeEntity(const glm::vec3 &pos, const std::shared_ptr<Texture> &texture, std::shared_ptr<BulletUniverse> world) :
        Cube(pos, texture),
        BulletObject(btVector3(pos.x,pos.y,pos.z), btQuaternion(0,0,0,1), new btBoxShape(btVector3(0.5,0.5,0.5))) {

    this->world = world;
    world->addRigidBody(this->getRigidBody());
}

CubeEntity::~CubeEntity() {
    this->world->removeRigidBody(this->getRigidBody());
}

void CubeEntity::think(std::chrono::duration<double, std::milli> delta) {
    btTransform bT = this->getTransformation();

    auto origin = bT.getOrigin();
    Cube::setOrigin(glm::vec3(origin.x(), origin.y(), origin.z()));

    //TODO transform rotation from bullet to opengl
}

void CubeEntity::setEntityPosition(const glm::vec3 &vec) {
    Cube::setOrigin(vec);
    BulletObject::setOrigin(btVector3(vec.x,vec.y,vec.z));
}
