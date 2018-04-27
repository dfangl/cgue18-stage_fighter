//
// Created by raphael on 19.03.18.
//

#include <algorithm>

#include <glad/glad.h>
#include <tiny_gltf.h>

#include "Model3DObject.h"
#include "../manager/TextureManager.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

Model3DObject::Model3DObject(const std::shared_ptr<tinygltf::Model> &model, const std::shared_ptr<Shader> &shader)
    : Object3D(shader) {

    this->gltfModel = model;
    glGenVertexArrays(1, &this->VAO);

    // Build Static VBOs of the gltf file:
    for (auto &bufferView : model->bufferViews) {
        tinygltf::Buffer &buffer = model->buffers[bufferView.buffer];

        if(bufferView.target == 0) {
            logger->warn("bufferView.target is 0, skipping entry");
            continue;
        }

        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(static_cast<GLenum>(bufferView.target), vbo);

        glBufferData(static_cast<GLenum>(bufferView.target),
                     bufferView.byteLength,
                     &buffer.data.at(0) + bufferView.byteOffset,
                     GL_STATIC_DRAW
        );
        glBindBuffer(static_cast<GLenum>(bufferView.target), 0);

        this->vbos.push_back(vbo);
    }

    rotation = glm::quat(1.0f,0.0f,0.0f,0.0f);

    // Prepare Static Node matrices:
    prepareModelMatrices();

    // Load Textures
    for (auto &texture : gltfModel->textures) {
        this->textures.push_back(TextureManager::load(gltfModel->images[texture.source], gltfModel->samplers[texture.sampler]));
    }

    if (textures.empty()) {
        spdlog::get("console")->critical(".glft Model did not contain any textures, loading fallback one");
        this->textures.push_back(TextureManager::load("wall.jpg"));
    }
}

void Model3DObject::draw() {
    //Support for more than one scene necessary?
    const auto &scene = this->gltfModel->scenes[gltfModel->defaultScene];

    for (auto &node : scene.nodes) {
        this->drawNode(node, gltfModel->nodes[node]);
    }
}

void Model3DObject::drawNode(const int idx, const tinygltf::Node &node) {
    if(node.mesh != -1) {
        auto &mesh = this->gltfModel->meshes[node.mesh];

        shader->setUniform("model", this->modelMatrix[idx]);
        shader->setUniform("nModel", this->normalMatrix[idx]);
        this->drawMesh(mesh);
    }
}

void Model3DObject::drawMesh(const tinygltf::Mesh &mesh) {
    for (auto &primitive : mesh.primitives) {
        if(primitive.indices < 0)
            return;

        // Process Material:
        auto &material = this->gltfModel->materials[primitive.material];
        if (!material.values["doubleSided"].bool_value) {
            glEnable(GL_CULL_FACE);
        } else {
            glDisable(GL_CULL_FACE);
        }

        auto emissiveFactor = material.values["emissiveFactor"].number_array;
        auto baseColorFactor = material.values["baseColorFactor"].number_array;
        auto metallicFactor = material.values["metallicFactor"].Factor();
        auto roughnessFactor = material.values["roughnessFactor"].Factor();

        /*
        if (emissiveFactor.empty()) {
            emissiveFactor.push_back(0.0f);
            emissiveFactor.push_back(0.0f);
            emissiveFactor.push_back(0.0f);
        }
         */

        const glm::vec3 baseColor = glm::vec3(baseColorFactor[0],baseColorFactor[1],baseColorFactor[2]);

        shader->setUniform("material.baseColor", baseColor);
        shader->setUniform("material.metallic", (float)metallicFactor);
        shader->setUniform("material.roughness", (float)roughnessFactor);

        // Bind Texture (Error?)
        // baseColorTexture is not set every time (exporter fuckup?)
        //const auto texId = material.values["baseColorTexture"].TextureIndex();
        auto &texture = this->textures[0];
        texture->bind(GL_TEXTURE0);

        shader->setUniform("texture_0", 0);

        glBindVertexArray(this->VAO);

        // Prepare attributes
        for (auto &attribute : primitive.attributes) {
            const tinygltf::Accessor &accessor = gltfModel->accessors[attribute.second];
            glBindBuffer(GL_ARRAY_BUFFER, this->vbos[accessor.bufferView]);

            size_t size;
            switch(accessor.type) {
                case TINYGLTF_TYPE_SCALAR: size = 1; break;
                case TINYGLTF_TYPE_VEC2: size = 2; break;
                case TINYGLTF_TYPE_VEC3: size = 3; break;
                case TINYGLTF_TYPE_VEC4: size = 4; break;
                default: throw std::runtime_error("Unknown accessor type!");
            }

            // Some of these attributes are read from gltf
            /*      JOINTS_0, NORMAL, POSITION, TEXCOORD_0, WEIGHTS_0 */
            std::string attrName = attribute.first;
            std::transform(attrName.begin(), attrName.end(), attrName.begin(), ::tolower);

            auto byteStride = accessor.ByteStride(gltfModel->bufferViews[accessor.bufferView]);
            this->shader->setVertexAttributePointer(attrName,
                                                    static_cast<GLuint>(size),
                                                    static_cast<GLenum>(accessor.componentType),
                                                    static_cast<GLboolean>(accessor.normalized ? GL_TRUE : GL_FALSE),
                                                    byteStride,
                                                    BUFFER_OFFSET(accessor.byteOffset));
        }

        const tinygltf::Accessor &indexAccess = gltfModel->accessors[primitive.indices];
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vbos[indexAccess.bufferView]);

        GLenum mode;
        switch(primitive.mode) {
            case TINYGLTF_MODE_TRIANGLES        : mode = GL_TRIANGLES;      break;
            case TINYGLTF_MODE_TRIANGLE_STRIP   : mode = GL_TRIANGLE_STRIP; break;
            case TINYGLTF_MODE_TRIANGLE_FAN     : mode = GL_TRIANGLE_FAN;   break;
            case TINYGLTF_MODE_POINTS           : mode = GL_POINTS;         break;
            case TINYGLTF_MODE_LINE             : mode = GL_LINE;           break;
            case TINYGLTF_MODE_LINE_LOOP        : mode = GL_LINE_LOOP;      break;
            default: throw std::runtime_error("Unknown primitive mode!");
        }

        glDrawElements(mode,
                       static_cast<GLsizei>(indexAccess.count),
                       static_cast<GLenum>(indexAccess.componentType),
                       BUFFER_OFFSET(indexAccess.byteOffset)
        );

        for (auto &attribute : primitive.attributes) {
            std::string attrName = attribute.first;
            std::transform(attrName.begin(), attrName.end(), attrName.begin(), ::tolower);

            this->shader->disableVAO(attrName);
        }

        glBindVertexArray(0);
    }
}

void Model3DObject::setOrigin(const glm::vec3 &vec) {
    this->translation = vec;
    updateModelMatrix();
}

void Model3DObject::prepareModelMatrices() {
    const tinygltf::Scene &scene = this->gltfModel->scenes[gltfModel->defaultScene];
    for (auto &nodeIndex : scene.nodes) {
        auto &node = gltfModel->nodes[nodeIndex];
        glm::mat4 worldMatrix(1.0f);

        // Local Modifications:
        // TODO: support Object Scaling
        worldMatrix = glm::translate(worldMatrix, this->translation);
        worldMatrix = worldMatrix * glm::toMat4(rotation);

        glm::mat4 modelMatrix(1.0f);

        // Gltf World:
        if(node.matrix.size() == 16) {
            glm::mat4 dataMat = glm::make_mat4(node.matrix.data());
            modelMatrix = modelMatrix * dataMat;
        } else {

            if(node.translation.size() == 3) {
                const glm::vec3 &t = glm::make_vec3(node.translation.data());
                modelMatrix = glm::translate(modelMatrix, t);
            }

            if(node.rotation.size() == 4) {
                const glm::quat &q = glm::quat(
                        static_cast<float>(node.rotation[3]),
                        static_cast<float>(node.rotation[0]),
                        static_cast<float>(node.rotation[1]),
                        static_cast<float>(node.rotation[2])
                );

                modelMatrix = modelMatrix * glm::toMat4(q);
            }

            if(node.scale.size() == 3) {
                const glm::vec3 &s = glm::make_vec3(node.scale.data());
                modelMatrix = glm::scale(modelMatrix, s);
            }
        }

        // model matrix = world matrix * gltf modelmatrix
        modelMatrix = worldMatrix * modelMatrix;
        glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));

        this->modelMatrix[nodeIndex] = modelMatrix;
        this->normalMatrix[nodeIndex] = normalMatrix;
    }
}

void Model3DObject::setRotation(const glm::quat &rot) {
    this->rotation = rot;
}
