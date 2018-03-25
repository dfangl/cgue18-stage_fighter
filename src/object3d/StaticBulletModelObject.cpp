//
// Created by raphael on 22.03.18.
//

#include <tiny_gltf.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "StaticBulletModelObject.h"

StaticBulletModelObject::StaticBulletModelObject(const btVector3 &pos, const btQuaternion &rotation, btScalar mass,
                                                 const std::shared_ptr<tinygltf::Model> &model,
                                                 const std::shared_ptr<Shader> &shader,
                                                 std::shared_ptr<BulletUniverse> &world)
        : BulletObject(pos, rotation, convertModelToTriangleMesh(model), 0),
          Model3DObject(model, shader) {

    this->world = world;

    //TODO: Populate alternative Models
    rigidBody->setFlags(rigidBody->getFlags() |
                                btCollisionObject::CF_KINEMATIC_OBJECT |
                                btCollisionObject::CF_STATIC_OBJECT
    );

    world->addCollsionObject(this->rigidBody);

    auto modelMesh = model->meshes[0];
    tinygltf::Node *node = nullptr;
    for( auto &n : model->nodes) {
        if(n.mesh == 0)
            node = &n;
    }

    if (node == nullptr) {
        spdlog::get("console")->error("Unable to find Node for Mesh, broken model?");
        throw std::runtime_error("Unable to find Node for Mesh, broken model?");
    }

    Object3D::setOrigin(glm::vec3(pos.x(), pos.y(), pos.z()));

    if(node->scale.size() == 3) {
        const glm::vec3 &s = glm::make_vec3(node->scale.data());
        BulletObject::setOrigin(pos * btVector3(s.x, s.y, s.z), rotation);
    } else {
        BulletObject::setOrigin(pos, rotation);
    }

}

btBvhTriangleMeshShape *StaticBulletModelObject::convertModelToTriangleMesh(const std::shared_ptr<tinygltf::Model> &model) {
    spdlog::get("console")->info("Mesh count: {}", model->meshes.size());

    auto modelMesh = model->meshes[0];
    tinygltf::Node *node = nullptr;
    for( auto &n : model->nodes) {
        if(n.mesh == 0)
            node = &n;
    }

    if (node == nullptr) {
        spdlog::get("console")->error("Unable to find Node for Mesh, broken model?");
        throw std::runtime_error("Unable to find Node for Mesh, broken model?");
    }

    auto *vertexArray = new btTriangleIndexVertexArray();


    const unsigned char * vertexPtr;
    const unsigned char * idxPtr;



    for (auto &primitive : modelMesh.primitives) {
        // Mesh must be triangles
        if (primitive.mode != TINYGLTF_MODE_TRIANGLES || primitive.indices < 0) {
            continue;
        }

        auto idxAccessor     = model->accessors[primitive.indices];
        auto indices         = model->bufferViews[idxAccessor.bufferView];
        auto &indexBuffer     = model->buffers[indices.buffer];
        auto indexByteStride = idxAccessor.ByteStride(indices);

        auto posAccessor     = model->accessors[primitive.attributes["POSITION"]];
        auto &vertices        = model->buffers[primitive.attributes["POSITION"]];
        auto posBufferView   = model->bufferViews[posAccessor.bufferView];
        auto posByteStride   = posAccessor.ByteStride(posBufferView);

        btIndexedMesh mIndexedMesh;
        {
            mIndexedMesh.m_numTriangles         = static_cast<int>(idxAccessor.count);
            mIndexedMesh.m_triangleIndexBase    = const_cast<unsigned char *>(&indexBuffer.data.at(0));// + indices.byteOffset;
            mIndexedMesh.m_triangleIndexStride  = indexByteStride;
            mIndexedMesh.m_numVertices          = static_cast<int>(posAccessor.count);
            mIndexedMesh.m_vertexBase           = const_cast<unsigned char *>(&vertices.data.at(0));// + posBufferView.byteOffset;
            mIndexedMesh.m_vertexStride         = posByteStride;

            // Totally useless in this bullet version:
            mIndexedMesh.m_indexType = PHY_SHORT;
            mIndexedMesh.m_vertexType = PHY_FLOAT;
        }


        vertexArray->addIndexedMesh(mIndexedMesh, PHY_SHORT);
    }

    auto *shape = new btBvhTriangleMeshShape(vertexArray, true);

    if(node->scale.size() == 3) {
        const glm::vec3 &s = glm::make_vec3(node->scale.data());
        shape->setLocalScaling(btVector3(s.x, s.y, s.z));
    }

    return shape;
}
