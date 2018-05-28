//
// Created by raphael on 22.03.18.
//

#include <tiny_gltf.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "StaticBulletModelObject.h"

StaticBulletModelObject::StaticBulletModelObject(const btVector3 &pos, float bsRadius,
                                                 const StaticModel &model,
                                                 const std::shared_ptr<Shader> &shader,
                                                 std::shared_ptr<BulletUniverse> &world)
        : BulletObject(pos, getModelRotationQuat(model.collision), convertModelToTriangleMesh(model.collision), 0),
          Model3DObject(glm::vec3(pos.x(), pos.y(), pos.z()), bsRadius, model.graphical, shader) {

    this->world = world;

    //TODO: Populate alternative Models
    rigidBody->setFlags(rigidBody->getFlags() |
                                btCollisionObject::CF_KINEMATIC_OBJECT |
                                btCollisionObject::CF_STATIC_OBJECT
    );

    world->addCollsionObject(this->rigidBody);
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


    auto *triangleMesh = new btTriangleMesh(false, false);
    auto *vertexArray = new btTriangleIndexVertexArray();

    spdlog::get("console")->info("Primitives  : {}", modelMesh.primitives.size());

    //for (auto &primitive : modelMesh.primitives) {
    auto &primitive = modelMesh.primitives[0];
        // Mesh must be triangles
        if (primitive.mode != TINYGLTF_MODE_TRIANGLES || primitive.indices < 0) {
            spdlog::get("console")->error("Unable to process given primitive, bullet might crash ...");
            //continue;
        }

        auto idxAccessor     = model->accessors[primitive.indices];
        auto indices         = model->bufferViews[idxAccessor.bufferView];
        auto &indexBuffer    = model->buffers[indices.buffer];
        auto indexByteStride = idxAccessor.ByteStride(indices);
        auto indexOffset     = idxAccessor.byteOffset + indices.byteOffset;

        auto posAccessor     = model->accessors[primitive.attributes["POSITION"]];
        auto vertices        = model->bufferViews[posAccessor.bufferView];
        auto &vertexBuffer   = model->buffers[primitive.attributes["POSITION"]];
        auto posByteStride   = posAccessor.ByteStride(vertices);
        auto vertexOffset    = posAccessor.byteOffset + vertices.byteOffset;

        const auto *idx = reinterpret_cast<unsigned short *>(&indexBuffer.data.at(indexOffset));
        const auto *vtx = reinterpret_cast<float *>(&vertexBuffer.data.at(vertexOffset));

    spdlog::get("console")->info("Indices  : {}", idxAccessor.count);
    spdlog::get("console")->info("Vertices : {}", posAccessor.count);
    spdlog::get("console")->info("Triangles: {}", idxAccessor.count / 3);

    /*
    for (int i=0; i<posAccessor.count; i+=9) {
        spdlog::get("console")->info("T:{} = {}, {}, {}", i/3, vtx[i+0], vtx[i+1], vtx[i+2]);
        spdlog::get("console")->info("T:{} = {}, {}, {}", i/3, vtx[i+1], vtx[i+2], vtx[i+3]);
        spdlog::get("console")->info("T:{} = {}, {}, {}", i/3, vtx[i+4], vtx[i+5], vtx[i+6]);

        triangleMesh->addTriangle(
                btVector3(vtx[i*3+0], vtx[i*3+1], vtx[i*3+2]),
                btVector3(vtx[i*3+3], vtx[i*3+4], vtx[i*3+5]),
                btVector3(vtx[i*3+6], vtx[i*3+7], vtx[i*3+8])
        );
    }
     */

    /*
    for (int i=0; i<idxAccessor.count; i+=3) {
        int v1 = idx[i+0] * 3;
        int v2 = idx[i+1] * 3;
        int v3 = idx[i+2] * 3;

        spdlog::get("console")->info("{} = {}, {}, {}", idx[i+0], vtx[v1+0], vtx[v1+1], vtx[v1+2]);
        spdlog::get("console")->info("{} = {}, {}, {}", idx[i+1], vtx[v2+0], vtx[v2+1], vtx[v2+2]);
        spdlog::get("console")->info("{} = {}, {}, {}", idx[i+2], vtx[v3+0], vtx[v3+1], vtx[v3+2]);

        triangleMesh->addTriangle(
                btVector3(vtx[v1+0], vtx[v1+1], vtx[v1+2]),
                btVector3(vtx[v2+0], vtx[v2+1], vtx[v2+2]),
                btVector3(vtx[v3+0], vtx[v3+1], vtx[v3+2])
        );
    }
     */


        btIndexedMesh mIndexedMesh;
        {
            mIndexedMesh.m_numTriangles         = static_cast<int>(idxAccessor.count / 3);
            mIndexedMesh.m_triangleIndexBase    = reinterpret_cast<const unsigned char *>(idx);
            mIndexedMesh.m_triangleIndexStride  = indexByteStride;
            mIndexedMesh.m_numVertices          = static_cast<int>(posAccessor.count);
            mIndexedMesh.m_vertexBase           = reinterpret_cast<const unsigned char *>(vtx);
            mIndexedMesh.m_vertexStride         = posByteStride;

            mIndexedMesh.m_vertexType = PHY_FLOAT;
            switch(posAccessor.componentType) {
                case TINYGLTF_COMPONENT_TYPE_FLOAT : mIndexedMesh.m_vertexType = PHY_FLOAT; break;
                case TINYGLTF_COMPONENT_TYPE_DOUBLE: mIndexedMesh.m_vertexType = PHY_DOUBLE; break;
                default:
                    spdlog::get("console")->critical("Bullet not compatible with gltf type, falling back to PHY_FLOAT! (vertexBuffer)");
            }

            mIndexedMesh.m_indexType = PHY_INTEGER;
            switch(idxAccessor.componentType) {
                case TINYGLTF_COMPONENT_TYPE_BYTE   : mIndexedMesh.m_indexType = PHY_UCHAR; break;
                case TINYGLTF_COMPONENT_TYPE_DOUBLE : mIndexedMesh.m_indexType = PHY_DOUBLE; break;
                case TINYGLTF_COMPONENT_TYPE_FLOAT  : mIndexedMesh.m_indexType = PHY_FLOAT; break;
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT : mIndexedMesh.m_indexType = PHY_SHORT; break;
                default:
                    spdlog::get("console")->critical("Bullet not compatible with gltf type, falling back to PHY_INTEGER! (indices)");
            }
        }

        vertexArray->addIndexedMesh(mIndexedMesh,  mIndexedMesh.m_indexType);
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
