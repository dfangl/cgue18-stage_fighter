//
// Created by raphael on 19.03.18.
//

#include <cmath>
#include <algorithm>
#include <cmath>

#include <glad/glad.h>
#include <tiny_gltf.h>

#include "Model3DObject.h"
#include "../manager/TextureManager.h"

#include "../helper/CompilerMacros.h"

#define BUFFER_OFFSET(t, i) ((t *)NULL + (i))

static std::string texture_name_vec[] = {
        "texture_0", "texture_1", "texture_2", "texture_3", "texture_4", "texture_5",
        "texture_6", "texture_7", "texture_8", "texture_9"
};

Model3DObject::Model3DObject(const glm::vec3 &position, float bsRadius,const std::shared_ptr<tinygltf::Model> &model,
                             const std::shared_ptr<Shader> &shader, int instances)
    : Object3D(position, bsRadius, shader) {
    this->instances = instances;
    this->gltfModel = model;

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

    // Reserve memory for instances:
    const auto instanceMemory = static_cast<unsigned long long int>(instances + 1);
    instancedModelMatrix.reserve(instanceMemory);
    instancedNormalMatrix.reserve(instanceMemory);
    instancedTranslation.reserve(instanceMemory);
    instancedRotation.reserve(instanceMemory);

    // Set position and rotation for at least the main instance of the object
    instancedTranslation.push_back(position);
    instancedRotation.emplace_back(1.0f, 0.0f, 0.0f, 0.0f);

    // Get Mesh count:
    const tinygltf::Scene &scene = this->gltfModel->scenes[gltfModel->defaultScene];
    const auto meshes = scene.nodes.size();

    // Check for only one mesh in scene,
    // this reduces complexity for instancing
    if (meshes > 1)
        throw std::runtime_error("Multiple meshes are not supported!");

    unsigned long nodeID = 0;
    for (nodeID = 0; gltfModel->nodes[nodeID].mesh != 0 && nodeID < scene.nodes.size(); nodeID++);
    if (nodeID == scene.nodes.size())
        throw std::runtime_error("Unable to find mesh with 0 in default scene!");

    this->gltfNodeIndex = nodeID;

    // Generate VAO and instanced VBO for the mesh
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &modelMatrixInstanceVBO);
    glGenBuffers(1, &normalMatrixInstanceVBO);

    auto &mesh = this->gltfModel->meshes[0];
    auto &primitive = mesh.primitives[0];

    // Error on a mesh without primitives
    if (primitive.indices < 0)
        throw std::runtime_error("This mesh does not have any primitives!");

    if (mesh.primitives.size() > 1)
        throw std::runtime_error("Mesh with multiple primitives is not supported!");

    glBindVertexArray(VAO);

    // Prepare attributes
    for (auto &attribute : primitive.attributes) {
        const tinygltf::Accessor &accessor = gltfModel->accessors[attribute.second];

        size_t size;
        switch(accessor.type) {
            case TINYGLTF_TYPE_SCALAR: size = 1; break;
            case TINYGLTF_TYPE_VEC2  : size = 2; break;
            case TINYGLTF_TYPE_VEC3  : size = 3; break;
            case TINYGLTF_TYPE_VEC4  : size = 4; break;
            default: throw std::runtime_error("Unknown accessor type!");
        }

        // Some of these attributes are read from gltf
        /*      JOINTS_0, NORMAL, POSITION, TEXCOORD_0, WEIGHTS_0 */
        std::string attrName = attribute.first;
        std::transform(attrName.begin(), attrName.end(), attrName.begin(), ::tolower);

        auto byteStride = accessor.ByteStride(gltfModel->bufferViews[accessor.bufferView]);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbos[accessor.bufferView]);
        this->shader->setVertexAttributePointer(attrName,
                                                static_cast<GLuint>(size),
                                                static_cast<GLenum>(accessor.componentType),
                                                static_cast<GLboolean>(accessor.normalized ? GL_TRUE : GL_FALSE),
                                                byteStride,
                                                BUFFER_OFFSET(char, accessor.byteOffset));
        if (instances > 0 )
            this->shader->setVertexAttribDivisor(attrName, 0);
    }

    // Bind EBO (indices) to the VAO
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

    meshDrawMode = mode;
    // Only to the following part if instances are used, otherwise these values will be set per uniform
    if (instances > 0) {
        const auto mLoc = shader->getAttribLocation("model");
        if (mLoc == -1)
            throw std::runtime_error("Unable to set instanced VAO attributes (sure that is the right shader?)");

        glBindBuffer(GL_ARRAY_BUFFER, modelMatrixInstanceVBO);
        shader->setVertexAttributePointer(mLoc + 0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(0 * sizeof(glm::vec4)));
        shader->setVertexAttributePointer(mLoc + 1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));
        shader->setVertexAttributePointer(mLoc + 2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        shader->setVertexAttributePointer(mLoc + 3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
        shader->setVertexAttribDivisor(mLoc, 4, 1);

        const auto nLoc = shader->getAttribLocation("nModel");
        if (nLoc == -1)
            throw std::runtime_error("Unable to set instanced VAO attributes (sure that is the right shader?)");


        glBindBuffer(GL_ARRAY_BUFFER, normalMatrixInstanceVBO);
        shader->setVertexAttributePointer(nLoc + 0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(0 * sizeof(glm::vec4)));
        shader->setVertexAttributePointer(nLoc + 1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));
        shader->setVertexAttributePointer(nLoc + 2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        shader->setVertexAttributePointer(nLoc + 3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
        shader->setVertexAttribDivisor(nLoc, 4, 1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glBindVertexArray(0);

    // Prepare Static Node matrices:
    this->nodeMatrix = getNodeMatrix();
    prepareModelMatrices(0);

    // Load Textures
    for (auto &texture : gltfModel->textures) {
        this->textures.push_back(TextureManager::load(gltfModel->images[texture.source], gltfModel->samplers[texture.sampler]));
    }

    /**
     * Note: The Shaders do not support texture less rendering, if supported delete this line:
     */
    if (textures.empty()) {
        spdlog::get("console")->critical(".glft Model did not contain any textures, loading fallback one");
        this->textures.push_back(TextureManager::load("wall.jpg"));
    }
}

void Model3DObject::draw() {
    // Re-construct the VBOs only if a instance has moved
    if (instances > 0 && recomputeInstanceBuffer) {
        // Push them to the GPU
        glBindBuffer(GL_ARRAY_BUFFER, modelMatrixInstanceVBO);
        glBufferData(GL_ARRAY_BUFFER, instancedTranslation.size() * sizeof(glm::mat4), instancedModelMatrix.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, normalMatrixInstanceVBO);
        glBufferData(GL_ARRAY_BUFFER, instancedTranslation.size() * sizeof(glm::mat4), instancedNormalMatrix.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        recomputeInstanceBuffer = false;
    }

    // Render all the instances with nodes to first mesh in default scene
    this->drawNode(gltfModel->nodes[gltfNodeIndex]);
}

void Model3DObject::drawNode(const tinygltf::Node &node) {
    //if(node.mesh != -1) {
    auto &mesh = this->gltfModel->meshes[node.mesh];

    // If no instances are used, set the uniforms since it's faster than instanced drawing
    if (instances == 0) {
        shader->setUniform("model", instancedModelMatrix[0]);
        shader->setUniform("nModel", instancedNormalMatrix[0]);
    }

    this->drawMesh(mesh, VAO, meshDrawMode);
    //}
}

void Model3DObject::drawMesh(const tinygltf::Mesh &mesh, GLuint &VAO, GLenum &mode) {
    auto &primitive = mesh.primitives[0];

    // Process Material (is this already parsed ?):
    auto &material = this->gltfModel->materials[primitive.material];
    if (!material.values["doubleSided"].bool_value)     glEnable(GL_CULL_FACE);
    else                                                glDisable(GL_CULL_FACE);

    auto emissiveFactor = material.values["emissiveFactor"].number_array;
    auto baseColorFactor = material.values["baseColorFactor"].number_array;
    auto metallicFactor = material.values["metallicFactor"].Factor();
    auto roughnessFactor = material.values["roughnessFactor"].Factor();

    const glm::vec3 baseColor = glm::vec3(baseColorFactor[0],baseColorFactor[1],baseColorFactor[2]);

    shader->setUniform("material.baseColor", baseColor);
    shader->setUniform("material.metallic", (float)metallicFactor);
    shader->setUniform("material.roughness", (float)roughnessFactor);

    // Bind Texture (Error?)
    // baseColorTexture is not set every time (exporter fuckup?)
    //const auto texId = material.values["baseColorTexture"].TextureIndex();
    for (size_t i=0; i<textures.size(); i++) {
        auto &texture = this->textures[i];
        texture->bind(static_cast<GLenum>(GL_TEXTURE0 + i));
        shader->setUniform(texture_name_vec[i], (GLint)i);
    }

    glBindVertexArray(VAO);

    // Byte offset is 0 which differs from buffer view byte offset -> wtf?
    const auto &indexAccess = gltfModel->accessors[primitive.indices];
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vbos[indexAccess.bufferView]);

    if (instances == 0)
        glDrawElements(mode,
                       static_cast<GLsizei>(indexAccess.count),
                       static_cast<GLenum>(indexAccess.componentType),
                       BUFFER_OFFSET(char, indexAccess.byteOffset)
        );
    else
        glDrawElementsInstanced(mode,
                                static_cast<GLsizei>(indexAccess.count),
                                static_cast<GLenum>(indexAccess.componentType),
                                BUFFER_OFFSET(char, indexAccess.byteOffset),
                                static_cast<GLsizei>(instancedTranslation.size())
        );

    glBindVertexArray(0);
}

void Model3DObject::setOrigin(const glm::vec3 &vec) {
   instancedTranslation[0] = vec;
   prepareModelMatrices(0);
}

void Model3DObject::prepareModelMatrices() {
    for (size_t i=0; i < instancedTranslation.size(); i++)
        this->prepareModelMatrices(i);
}

void Model3DObject::setRotation(const glm::quat &rot) {
    instancedRotation[0] = rot;
}

unsigned int Model3DObject::addInstance(const glm::vec3 &vec, const glm::quat &rot) {
    const auto ID  = this->instanceID++;
    const auto end = instancedTranslation.size();
    instanceMapping[ID] = end;

    instancedTranslation.push_back(vec);
    instancedRotation.push_back(rot);
    instancedModelMatrix.emplace_back(1.0f);
    instancedNormalMatrix.emplace_back(1.0f);

    prepareModelMatrices(end);
    return ID;
}

// faulty?
void Model3DObject::removeInstance(unsigned int id) {
    const auto old = instanceMapping[id];
    const auto end = instancedTranslation.size() - 1;
    unsigned long lastElementID = 0;

    // find lastElementID element:
    for (const auto &inst : instanceMapping) {
        if (inst.second == instancedTranslation.size() - 1) {
            lastElementID = inst.first;
            break;
        }
    }

    // re-map the instance
    instanceMapping[lastElementID]  = old;
    instancedTranslation[old]       = instancedTranslation[end];
    instancedRotation[old]          = instancedRotation[end];

    // shrink arrays:
    instancedTranslation.pop_back();
    instancedRotation.pop_back();
    instancedNormalMatrix.pop_back();
    instancedModelMatrix.pop_back();

    // matrices are going to be recomputed
    recomputeInstanceBuffer = true;
    instanceMapping.erase(id);
}

void Model3DObject::setInstance(unsigned int id, const glm::vec3 &vec, const glm::quat &rot) {
    const auto &pos = instanceMapping[id];
    instancedTranslation[pos] = vec;
    instancedRotation[pos] = rot;

    prepareModelMatrices(static_cast<unsigned long>(pos));
}

glm::mat4 Model3DObject::getNodeMatrix() {
    auto &node = gltfModel->nodes[this->gltfNodeIndex];
    glm::mat4 modelMatrix(1.0f);

    // Gltf World:
    if(node.matrix.size() == 16) {
        glm::mat4 dataMat = glm::make_mat4(node.matrix.data());
        modelMatrix = modelMatrix * dataMat;
    } //else { (removed due animation support)

    // Extract translation and rotation in the gltf
    // scene with the correct animation timestamp
    if(node.translation.size() == 3 && !this->animDataInternal.inAnimation) {
        const glm::vec3 &t = glm::make_vec3(node.translation.data());
        modelMatrix = glm::translate(modelMatrix, t);
    } else if (this->animDataInternal.inAnimation) {
        modelMatrix = glm::translate(modelMatrix, getAnimationTranslation());
    }


    if(node.rotation.size() == 4 && !this->animDataInternal.inAnimation) {
        const glm::quat &q = glm::quat(
                static_cast<float>(node.rotation[3]),
                static_cast<float>(node.rotation[0]),
                static_cast<float>(node.rotation[1]),
                static_cast<float>(node.rotation[2])
        );

        modelMatrix = modelMatrix * glm::toMat4(q);
    } else if(this->animDataInternal.inAnimation) {
        modelMatrix = modelMatrix * glm::toMat4(getAnimationRotation());
    }


    if(node.scale.size() == 3) {
        const glm::vec3 &s = glm::make_vec3(node.scale.data());
        modelMatrix = glm::scale(modelMatrix, s);
    }

    return modelMatrix;
}

void Model3DObject::prepareModelMatrices(const unsigned long pos) {
    recomputeInstanceBuffer = true;
    glm::mat4 worldMatrix(1.0f);

    // Local Modifications:
    // TODO: support Object Scaling
    worldMatrix = glm::translate(worldMatrix, instancedTranslation[pos]);
    worldMatrix = worldMatrix * glm::toMat4(instancedRotation[pos]); // <-- maybe save as mat4 ?

    // Apply world and model matrix
    glm::mat4 modelMatrix  = worldMatrix * this->nodeMatrix;
    glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));

    instancedModelMatrix[pos] = modelMatrix;
    instancedNormalMatrix[pos] = normalMatrix;
}

void Model3DObject::clearInstances() {
    this->instancedModelMatrix.clear();
    this->instancedNormalMatrix.clear();
    this->instancedRotation.clear();
    this->instancedTranslation.clear();
}

void Model3DObject::enableAnimation(const Model3DObject::Animation &data) {
    this->animationData = data;
    this->animDataInternal.currentAnimationTime = data.startTime;
    this->animDataInternal.inAnimation = true;

    const auto &animations = this->gltfModel->animations;
    int animationIndex = -1;

    for(size_t i=0; i<animations.size(); i++) {
        if (data.name == animations[i].name) {
            animationIndex = static_cast<int>(i);
            break;
        }
    }

    if (animationIndex == -1)
        throw std::runtime_error("Unable to find Animation!");

    std::map<std::string, int> samplers;
    for (const auto &channel : animations[animationIndex].channels)
        samplers[channel.target_path] = channel.sampler;

    this->animDataInternal.translation = animations[animationIndex].samplers[samplers["translation"]];
    this->animDataInternal.rotation    = animations[animationIndex].samplers[samplers["rotation"]];
}

void Model3DObject::disableAnimation() {
    this->animDataInternal.inAnimation = false;
}

void Model3DObject::applyAnimation(float currentTime) {
    if (animationData.loop) {
        const auto time = this->animDataInternal.currentAnimationTime + currentTime;

        if (time > animationData.endTime) {
            animDataInternal.currentAnimationTime =
                    animationData.startTime + time - animationData.endTime;
        }  else {
            animDataInternal.currentAnimationTime += currentTime;
        }
    } else if (currentTime < (animationData.startTime - animationData.endTime)){
        this->animDataInternal.currentAnimationTime = currentTime;
    }

    this->nodeMatrix = getNodeMatrix();
    this->prepareModelMatrices();
}

glm::vec3 Model3DObject::getAnimationTranslation() {
    const auto &timeAccessor = this->gltfModel->accessors[animDataInternal.translation.input];
    const auto &translationAccessor = this->gltfModel->accessors[animDataInternal.translation.output];

    const auto &timeBufferView = this->gltfModel->bufferViews[timeAccessor.bufferView];
    const auto &translationBufferView = this->gltfModel->bufferViews[translationAccessor.bufferView];

    const auto &timeBuffer = this->gltfModel->buffers[timeBufferView.buffer];
    const auto &translationBuffer = this->gltfModel->buffers[translationBufferView.buffer];

    const auto *time = reinterpret_cast<const float *>(timeBuffer.data.data());
    const auto *translationPtr = translationBuffer.data.data() + translationBufferView.byteOffset;

    const auto &curTime = animDataInternal.currentAnimationTime;
    float prevTime = time[0];
    float nextTime = time[timeAccessor.count-1];
    unsigned int i;

    for (i=0; curTime > time[i] && i < timeAccessor.count - 2; i++);
    prevTime = time[i];
    nextTime = time[i+1];

    const auto *prevTransPtr = reinterpret_cast<const float *>(translationPtr + i * translationBufferView.byteStride);
    const auto *nextTransPtr = reinterpret_cast<const float *>(translationPtr + (i+1) * translationBufferView.byteStride);

    const glm::vec3 prevTranslation = glm::make_vec3(prevTransPtr);
    const glm::vec3 nextTranslation = glm::make_vec3(nextTransPtr);

    const auto transDelta = nextTranslation - prevTranslation;
    const auto timeDelta = ((curTime - prevTime) / (nextTime - prevTime));
    const auto result = prevTranslation + (transDelta * timeDelta);

    if (curTime >= 1.23953) {
        spdlog::get("console")->info("next: {},{},{}", nextTranslation.x, nextTranslation.y, nextTranslation.z);
        spdlog::get("console")->info("prev: {},{},{}", prevTranslation.x, prevTranslation.y, prevTranslation.z);
        spdlog::get("console")->info("rotation: {},{},{}", result.x, result.y, result.z);
        spdlog::get("console")->info("deltaTime: {}", timeDelta);
    }


    return result;
}

glm::quat Model3DObject::getAnimationRotation() {
    const auto &timeAccessor = this->gltfModel->accessors[animDataInternal.rotation.input];
    const auto &rotationAccessor = this->gltfModel->accessors[animDataInternal.rotation.output];

    const auto &timeBufferView = this->gltfModel->bufferViews[timeAccessor.bufferView];
    const auto &rotationBufferView = this->gltfModel->bufferViews[rotationAccessor.bufferView];

    const auto &timeBuffer = this->gltfModel->buffers[timeBufferView.buffer];
    const auto &rotationBuffer = this->gltfModel->buffers[rotationBufferView.buffer];

    const auto *time = reinterpret_cast<const float *>(timeBuffer.data.data());
    const auto *rotationPtr = rotationBuffer.data.data() + rotationBufferView.byteOffset;

    const auto &curTime = animDataInternal.currentAnimationTime;
    float prevTime = time[0];
    float nextTime = time[timeAccessor.count-1];
    unsigned int i;

    for (i=0; curTime > time[i] && i < timeAccessor.count - 2; i++);
    prevTime = time[i];
    nextTime = time[i+1];

    //spdlog::get("console")->info("Time:{}, i:{}, next:{}, max:{}", curTime, i, i+1, timeAccessor.count);

    const auto *prevRotPtr = reinterpret_cast<const float *>(rotationPtr + i * rotationBufferView.byteStride);
    const auto *nextRotPtr = reinterpret_cast<const float *>(rotationPtr + (i+1) * rotationBufferView.byteStride);

    const glm::vec4 prevRotation = glm::make_vec4(prevRotPtr);
    const glm::vec4 nextRotation = glm::make_vec4(nextRotPtr);

    // error @ frametime 1.23953 sec ?


    const auto transDelta = nextRotation - prevRotation;
    const auto timeDelta = ((curTime - prevTime) / (nextTime - prevTime));
    const auto rotVector = prevRotation + (transDelta * timeDelta);

    if (curTime >= 1.23953) {
        spdlog::get("console")->info("next: {},{},{},{}", nextRotation.w, nextRotation.x, nextRotation.y, nextRotation.z);
        spdlog::get("console")->info("prev: {},{},{},{}", prevRotation.w, prevRotation.x, prevRotation.y, prevRotation.z);
        spdlog::get("console")->info("rotation: {},{},{},{}", -rotVector.w, rotVector.x, rotVector.y, rotVector.z);
        spdlog::get("console")->info("deltaTime: {}", timeDelta);
    }


    //spdlog::get("console")->info("rotation: {},{},{},{}", -rotVector.w, rotVector.x, rotVector.y, rotVector.z);

    const auto result = glm::quat(rotVector.w, rotVector.x, rotVector.y, rotVector.z);
    return result;
}
