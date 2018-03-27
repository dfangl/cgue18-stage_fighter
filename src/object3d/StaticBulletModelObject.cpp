//
// Created by raphael on 22.03.18.
//

#include <tiny_gltf.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "StaticBulletModelObject.h"

StaticBulletModelObject::StaticBulletModelObject(const btVector3 &pos,
                                                 const StaticModel &model,
                                                 const std::shared_ptr<Shader> &shader,
                                                 std::shared_ptr<BulletUniverse> &world)
        : BulletObject(pos, getModelRotationQuat(model.collision), convertModelToTriangleMesh(model.collision), 0),
          Model3DObject(model.graphical, shader) {

    this->world = world;

    //TODO: Populate alternative Models
    rigidBody->setFlags(rigidBody->getFlags() |
                                btCollisionObject::CF_KINEMATIC_OBJECT |
                                btCollisionObject::CF_STATIC_OBJECT
    );

    world->addCollsionObject(this->rigidBody);
    Model3DObject::setOrigin(glm::vec3(pos.x(), pos.y(), pos.z()));
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

    //auto *triangleMesh = new btTriangleMesh(false, false);
    auto *vertexArray = new btTriangleIndexVertexArray();

    //for (auto &primitive : modelMesh.primitives) {
    auto &primitive = modelMesh.primitives[0];
        // Mesh must be triangles
        if (primitive.mode != TINYGLTF_MODE_TRIANGLES || primitive.indices < 0) {
            spdlog::get("console")->error("Unable to process given primitive, bullet might crash ...");
        //    continue;
        }

        auto idxAccessor     = model->accessors[primitive.indices];
        auto indices         = model->bufferViews[idxAccessor.bufferView];
        auto &indexBuffer    = model->buffers[indices.buffer];
        auto indexByteStride = idxAccessor.ByteStride(indices);

        auto posAccessor     = model->accessors[primitive.attributes["POSITION"]];
        auto &vertices       = model->buffers[primitive.attributes["POSITION"]];
        auto posBufferView   = model->bufferViews[posAccessor.bufferView];
        auto posByteStride   = posAccessor.ByteStride(posBufferView);

        btIndexedMesh mIndexedMesh;
        {
            mIndexedMesh.m_numTriangles         = static_cast<int>(idxAccessor.count / 3) ;
            mIndexedMesh.m_triangleIndexBase    = const_cast<unsigned char *>(&indexBuffer.data.at(0)) + indices.byteOffset;
            mIndexedMesh.m_triangleIndexStride  = indexByteStride;
            mIndexedMesh.m_numVertices          = static_cast<int>(posAccessor.count);
            mIndexedMesh.m_vertexBase           = const_cast<unsigned char *>(&vertices.data.at(0)) + posBufferView.byteOffset;
            mIndexedMesh.m_vertexStride         = posByteStride;

            // Totally useless in this bullet version:
            //mIndexedMesh.m_indexType = PHY_SHORT;

            mIndexedMesh.m_vertexType = PHY_FLOAT;
            switch(posAccessor.componentType) {
                case TINYGLTF_COMPONENT_TYPE_FLOAT : mIndexedMesh.m_vertexType = PHY_FLOAT; break;
                default:
                    spdlog::get("console")->critical("Bullet not compatible with gltf type, falling back to PHY_FLOAT! (vertices)");
            }
        }

        PHY_ScalarType type = PHY_INTEGER;
        switch(idxAccessor.componentType) {
            case TINYGLTF_COMPONENT_TYPE_BYTE   : type = PHY_UCHAR; break;
            case TINYGLTF_COMPONENT_TYPE_DOUBLE : type = PHY_DOUBLE; break;
            case TINYGLTF_COMPONENT_TYPE_FLOAT  : type = PHY_FLOAT; break;
            case TINYGLTF_COMPONENT_TYPE_SHORT  : type = PHY_SHORT; break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT : type = PHY_SHORT; break;
            default:
                spdlog::get("console")->critical("Bullet not compatible with gltf type, falling back to PHY_INTEGER! (indices)");
        }

        vertexArray->addIndexedMesh(mIndexedMesh, type);
    //}

    if(node->scale.size() == 3) {
        const glm::vec3 &s = glm::make_vec3(node->scale.data());
        vertexArray->setScaling(btVector3(s.x, s.y, s.z));
    }

    return new btBvhTriangleMeshShape(vertexArray, true);
}

btQuaternion StaticBulletModelObject::getModelRotationQuat(const std::shared_ptr<tinygltf::Model> &model) {
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

    if (node->rotation.size() == 4)
        return btQuaternion(
                static_cast<const btScalar &>(node->rotation[0]),
                static_cast<const btScalar &>(node->rotation[1]),
                static_cast<const btScalar &>(node->rotation[2]),
                static_cast<const btScalar &>(node->rotation[3])
        );

    return btQuaternion(0, 0, 0, 1);
}
