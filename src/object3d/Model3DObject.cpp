//
// Created by raphael on 19.03.18.
//

#include <algorithm>

#include <glad/glad.h>
#include <tiny_gltf.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include "Model3DObject.h"
#include "../manager/TextureManager.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

Model3DObject::Model3DObject(const std::shared_ptr<tinygltf::Model> &model, const std::shared_ptr<Shader> &shader)
    : Object3D(shader) {

    this->gltfModel = model;

    for( auto &bufferView : model->bufferViews ) {
        tinygltf::Buffer &buffer = model->buffers[bufferView.buffer];

        if(bufferView.target == 0) {
            logger->warn("bufferView.target is 0, skipping entry");
            continue;
        }

        GLuint vbo, vao;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindBuffer(static_cast<GLenum>(bufferView.target), vbo);

        glBufferData(static_cast<GLenum>(bufferView.target),
                     bufferView.byteLength,
                     &buffer.data.at(0) + bufferView.byteOffset,
                     GL_STATIC_DRAW
        );
        logger->info("glBufferData:\t{}", glGetError());

        logger->info("vbo={}\tglBindBufferData({}, {}, {}, GL_STATIC_DRAW)",vbo, bufferView.target, bufferView.byteLength, (void *)( &buffer.data.at(0) + bufferView.byteOffset));
        float *RAW_BUFFER = reinterpret_cast<float *>(&buffer.data.at(0) + bufferView.byteOffset);
        for(int i=0; i<bufferView.byteLength/sizeof(float); i+=3) {
            logger->info("{},  {}, {}", RAW_BUFFER[i], RAW_BUFFER[i+1], RAW_BUFFER[i+3]);
        }


        glBindBuffer(static_cast<GLenum>(bufferView.target), 0);

        this->vbos.push_back(vbo);
        this->vao.push_back(vao);
    }

    //TODO: Material and Texture loading
    this->texture0 = TextureManager::load("wall.jpg");
}

void Model3DObject::draw() {
    //Support for more than one scene necessary?

    const tinygltf::Scene &scene = this->gltfModel->scenes[gltfModel->defaultScene];
    for (auto &node : gltfModel->nodes) {
        this->drawNode(node);
    }

}

void Model3DObject::drawNode(const tinygltf::Node &node) {
    glm::mat4 modelMatrix(1.0);

    if(node.matrix.size() == 16)
        modelMatrix = glm::make_mat4(node.matrix.data());
    else {

        if(node.scale.size() == 3) {
            const glm::vec3 &s = glm::make_vec3(node.scale.data());
            modelMatrix = glm::scale(modelMatrix, s);
        }

        if(node.rotation.size() == 4) {
            const glm::vec3 &r = glm::make_vec3(&node.rotation.at(1));
            const auto angle = static_cast<const float>(node.rotation.at(0));
            modelMatrix = glm::rotate(modelMatrix, angle, r);
        }

        if(node.translation.size() == 3) {
            const glm::vec3 &t = glm::make_vec3(node.translation.data());
            modelMatrix = glm::translate(modelMatrix, t);
        }
    }

    shader->setUniform("model", this->model);
    texture0->bind(GL_TEXTURE0);
    shader->setUniform("texture_0", 0);

    if(node.mesh != -1)
        this->drawMesh(this->gltfModel->meshes[node.mesh]);

    for (auto &child : node.children) {
        this->drawNode(gltfModel->nodes[child]);
    }
}

void Model3DObject::drawMesh(const tinygltf::Mesh &mesh) {
    for (auto &primitive : mesh.primitives) {
        if(primitive.indices < 0)
            return;

        // Prepare attributes
        for (auto &attribute : primitive.attributes) {
            const tinygltf::Accessor &accessor = gltfModel->accessors[attribute.second];
            glBindVertexArray(this->vao[accessor.bufferView]);
            glBindBuffer(GL_ARRAY_BUFFER, this->vbos[accessor.bufferView]);
            logger->info("glBindBuffer({}): \t{}", this->vbos[accessor.bufferView], glGetError());

            size_t size;
            switch(accessor.type) {
                case TINYGLTF_TYPE_SCALAR: size = 1; break;
                case TINYGLTF_TYPE_VEC2: size = 2; break;
                case TINYGLTF_TYPE_VEC3: size = 3; break;
                case TINYGLTF_TYPE_VEC4: size = 4; break;
                default: throw std::runtime_error("Unknown accessor type!");
            }

            // Some of these attributes are read from gltf
            /*      "JOINTS_0"
                    "NORMAL"
                    "POSITION"
                    "TEXCOORD_0"
                    "WEIGHTS_0"
            */
            std::string attrName = attribute.first;
            std::transform(attrName.begin(), attrName.end(), attrName.begin(), ::tolower);

            auto byteStide = accessor.ByteStride(gltfModel->bufferViews[accessor.bufferView]);
            this->shader->setVertexAttributePointer(attrName,
                                                    static_cast<GLuint>(size),
                                                    static_cast<GLenum>(accessor.componentType),
                                                    static_cast<GLboolean>(accessor.normalized ? GL_TRUE : GL_FALSE),
                                                    byteStide,
                                                    BUFFER_OFFSET(accessor.byteOffset));
        }

        const tinygltf::Accessor &indexAccess = gltfModel->accessors[primitive.indices];
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vbos[indexAccess.bufferView]);
        logger->info("glBindBuffer2\t{}", glGetError());

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

        logger->info("glDrawElements({}, {}, {}): \t{}", indexAccess.count, indexAccess.componentType, (void*)BUFFER_OFFSET(indexAccess.byteOffset) ,glGetError());


        for (auto &attribute : primitive.attributes) {
            std::string attrName = attribute.first;
            std::transform(attrName.begin(), attrName.end(), attrName.begin(), ::tolower);

            this->shader->disableVAO(attrName);
        }

        glBindVertexArray(0);
    }
}
