//
// Created by raphael on 22.03.18.
//

#include <tiny_gltf.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "StaticBulletModelObject.h"

StaticBulletModelObject::StaticBulletModelObject(const btVector3 &pos, const btQuaternion &rotation, btScalar mass,
                                                 const std::shared_ptr<tinygltf::Model> &model, const std::shared_ptr<Shader> &shader)
        : BulletObject(pos, rotation, convertModelToTriangleMesh(model), 0),
          Model3DObject(model, shader) {

    //TODO: Populate alternative Models

    Object3D::setOrigin(glm::vec3(pos.x(), pos.y(), pos.z()));
    this->collisionObject = new btCollisionObject();
    this->collisionObject->setCollisionShape(rigidBody->getCollisionShape());
    //rigidBody->setFlags( rigidBody->getFlags() | btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_STATIC_OBJECT);
}

btBvhTriangleMeshShape *StaticBulletModelObject::convertModelToTriangleMesh(const std::shared_ptr<tinygltf::Model> &model) {
    btTriangleMesh *bulletMesh = new btTriangleMesh;

    spdlog::get("console")->info("Mesh count: {}", model->meshes.size());
    auto modelMesh = model->meshes[0];
    tinygltf::Node *node = nullptr;
    for( auto &n : model->nodes) {
        if(n.mesh == 0)
            node = &n;
    }

    spdlog::get("console")->info("Node name: {}", node->name);

    glm::mat4 modelMatrix(1.0);

    if(node->matrix.size() == 16)
        modelMatrix = glm::make_mat4(node->matrix.data());
    else {

        if(node->scale.size() == 3) {
            const glm::vec3 &s = glm::make_vec3(node->scale.data());
            modelMatrix = glm::scale(modelMatrix, s);
        }

        if(node->rotation.size() == 4) {
            const glm::vec3 &r = glm::make_vec3(&node->rotation.at(1));
            const auto angle = static_cast<const float>(node->rotation.at(0));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), r);
        }

        if(node->translation.size() == 3) {
            const glm::vec3 &t = glm::make_vec3(node->translation.data());
            modelMatrix = glm::translate(modelMatrix, t);
        }
    }


    auto primitive = modelMesh.primitives[0];
    //for (auto &primitive:modelMesh.primitives) {
        spdlog::get("console")->info("primitive.indices: {}", primitive.indices);

        //if(primitive.indices < 0)
        //    continue;

    const tinygltf::Accessor &indexAccess = model->accessors[primitive.indices];
    auto indices = model->bufferViews[indexAccess.bufferView];
    tinygltf::Buffer &indexBuffer = model->buffers[indices.buffer];
    auto indexByteStride = indexAccess.ByteStride(indices);

    auto triangles = model->buffers[primitive.attributes["POSITION"]];

    auto *vertexArray = new btTriangleIndexVertexArray(
            static_cast<int>(triangles.data.size() / 3),
            reinterpret_cast<int *>(&indexBuffer.data.at(0) + indices.byteOffset),
            0,
            static_cast<int>(triangles.data.size()),
            reinterpret_cast<btScalar *>(&triangles.data.at(0)),
            sizeof(float)*3
    );


    //TODO: Check if primitive is triangle and float arrays!

/*
    for(int i=0; i<positions.data.size(); i+=9) {
        glm::vec4 v1 = modelMatrix * glm::vec4(positions.data[i+0], positions.data[i+1], positions.data[i+2], 1);
        glm::vec4 v2 = modelMatrix * glm::vec4(positions.data[i+3], positions.data[i+4], positions.data[i+5], 1);
        glm::vec4 v3 = modelMatrix * glm::vec4(positions.data[i+6], positions.data[i+7], positions.data[i+8], 1);

        bulletMesh->addTriangle(
                btVector3(v1.x, v1.y, v1.z),
                btVector3(v2.x, v2.y, v2.z),
                btVector3(v3.x, v3.y, v3.z)
        );
    }
    //}
*/

    auto *shape = new btBvhTriangleMeshShape(vertexArray, true);
    //shape->setLocalScaling(btVector3(node->scale[0],node->scale[1],node->scale[2]));

    return shape;
}
