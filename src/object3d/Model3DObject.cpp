//
// Created by raphael on 19.03.18.
//

#include <algorithm>

#include <glad/glad.h>
#include <tiny_gltf.h>

#include "Model3DObject.h"
#include "../manager/TextureManager.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

Model3DObject::Model3DObject(const std::shared_ptr<tinygltf::Model> &model, const std::shared_ptr<Shader> &shader, int instances)
    : Object3D(shader) {
    this->instances = instances;
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

    modelMatrix.push_back(Matrix(0, glm::vec3(0,0,0), glm::quat(1.0f,0.0f,0.0f,0.0f)));

    const tinygltf::Scene &scene = this->gltfModel->scenes[gltfModel->defaultScene];
    const auto meshes = scene.nodes.size();

    modelMatrixInstanceVBO.reserve(meshes);
    glGenBuffers(meshes, modelMatrixInstanceVBO.data());

    normalMatrixInstanceVBO.reserve(meshes);
    glGenBuffers(meshes, normalMatrixInstanceVBO.data());

    for (auto &nodeIndex : scene.nodes) {
        std::vector<glm::mat4> instanceBuffer;
        std::vector<glm::mat4> normalBuffer;

        instanceBuffer.reserve(instances + 1);
        normalBuffer.reserve(instances + 1);

        instancedModelMatrix.push_back(instanceBuffer);
        instancedNormalMatrix.push_back(normalBuffer);
    }

    if (instances > 0) {
        glBindVertexArray(this->VAO);

        const auto mLoc = shader->getLocation("model");
        const auto nLoc = shader->getLocation("nModel");
        shader->setVertexAttributePointer(mLoc + 0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(0 * sizeof(glm::vec4)));
        shader->setVertexAttributePointer(mLoc + 1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));
        shader->setVertexAttributePointer(mLoc + 2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        shader->setVertexAttributePointer(mLoc + 3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        shader->setVertexAttributePointer(nLoc + 0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(0 * sizeof(glm::vec4)));
        shader->setVertexAttributePointer(nLoc + 1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));
        shader->setVertexAttributePointer(nLoc + 2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        shader->setVertexAttributePointer(nLoc + 3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        shader->setVertexAttribDivisor(mLoc, 4, 1);
        shader->setVertexAttribDivisor(nLoc, 4, 1);

        glBindVertexArray(0);
    }


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

    // Re-construct the VBOs only if a instance has moved
    if (instances > 0 && recomputeInstanceBuffer) {
        instancedModelMatrix.clear();
        instancedNormalMatrix.clear();

        // Build instanced VBO Data of model / normal matrices
        for (auto &instance : this->modelMatrix) {
            for (int i = 0; i<instance.modelMatrix.size(); i++)
                instancedModelMatrix[i].push_back(instance.modelMatrix[i]);

            for (int i = 0; i<instance.normalMatrix.size(); i++)
                instancedNormalMatrix[i].push_back(instance.normalMatrix[i]);
        }

        // Push them to the GPU
        for (int i=0; i<instancedModelMatrix.size(); i++) {
            glBindBuffer(GL_ARRAY_BUFFER, modelMatrixInstanceVBO[i]);
            glBufferData(GL_ARRAY_BUFFER, modelMatrix.size() * sizeof(glm::mat4), instancedModelMatrix.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, normalMatrixInstanceVBO[i]);
            glBufferData(GL_ARRAY_BUFFER, modelMatrix.size() * sizeof(glm::mat4), normalMatrixInstanceVBO.data(), GL_STATIC_DRAW);
        }
    }

    // Render all the instances with all nodes and meshes ...
    for (auto &instance : this->modelMatrix)
        for (auto &node : scene.nodes) {
            this->drawNode(node, gltfModel->nodes[node], instance);
        }
}

void Model3DObject::drawNode(const int idx, const tinygltf::Node &node, Matrix &instance) {
    if(node.mesh != -1) {
        auto &mesh = this->gltfModel->meshes[node.mesh];

        if (instances == 0) {
            shader->setUniform("model", instance.modelMatrix[idx]);
            shader->setUniform("nModel", instance.normalMatrix[idx]);
        }
        this->drawMesh(mesh);
    }
}

void Model3DObject::drawMesh(const tinygltf::Mesh &mesh) {
    char texNameBuffer[16];

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
        for (int i=0; i<textures.size(); i++) {
            auto &texture = this->textures[i];
            texture->bind(GL_TEXTURE0 + i);
            snprintf(texNameBuffer, 16, "texture_%d", i);
            shader->setUniform(texNameBuffer, i);
        }

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

        if (activeInstances == 0)
            glDrawElements(mode,
                           static_cast<GLsizei>(indexAccess.count),
                           static_cast<GLenum>(indexAccess.componentType),
                           BUFFER_OFFSET(indexAccess.byteOffset)
            );
        else
            glDrawElementsInstanced(mode,
                                    static_cast<GLsizei>(indexAccess.count),
                                    static_cast<GLenum>(indexAccess.componentType),
                                    BUFFER_OFFSET(indexAccess.byteOffset),
                                    activeInstances
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
    modelMatrix[0].translation = vec;
    prepareModelMatrices(modelMatrix[0]);
}

void Model3DObject::prepareModelMatrices() {
    const tinygltf::Scene &scene = this->gltfModel->scenes[gltfModel->defaultScene];

    for (auto &instance : this->modelMatrix)
        this->prepareModelMatrices(instance);
}

void Model3DObject::setRotation(const glm::quat &rot) {
    modelMatrix[0].rotation = rot;
}

unsigned int Model3DObject::addInstance(const glm::vec3 &vec, const glm::quat &rot) {
    modelMatrix.push_back(Matrix(
        this->instanceID++,
        vec,
        rot
    ));

    return this->instanceID;
}

void Model3DObject::removeInstance(unsigned int id) {

}

void Model3DObject::setInstance(unsigned int  id, const glm::vec3 &vec, const glm::quat &rot) {

}

void Model3DObject::prepareModelMatrices(Model3DObject::Matrix &instance) {
    recomputeInstanceBuffer = true;
    const tinygltf::Scene &scene = this->gltfModel->scenes[gltfModel->defaultScene];

    instance.modelMatrix.clear();
    instance.normalMatrix.clear();

    for (auto &nodeIndex : scene.nodes) {
        auto &node = gltfModel->nodes[nodeIndex];
        glm::mat4 worldMatrix(1.0f);

        // Local Modifications:
        // TODO: support Object Scaling
        worldMatrix = glm::translate(worldMatrix, instance.translation);
        worldMatrix = worldMatrix * glm::toMat4(instance.rotation);

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

        instance.modelMatrix.push_back(modelMatrix);
        instance.normalMatrix.push_back(normalMatrix);
    }
}
