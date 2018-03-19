//
// Created by raphael on 19.03.18.
//


#include <glad/glad.h>
#include <tiny_gltf.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include "Model3DObject.h"

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

    //TODO: Material and Texture loading
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

    shader->setUniform("model", modelMatrix);
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
            glBindBuffer(GL_ARRAY_BUFFER, this->vbos[accessor.bufferView]);

            size_t size;
            switch(accessor.type) {
                case TINYGLTF_TYPE_SCALAR: size = 1; break;
                case TINYGLTF_TYPE_VEC2: size = 2; break;
                case TINYGLTF_TYPE_VEC3: size = 3; break;
                case TINYGLTF_TYPE_VEC4: size = 4; break;
                default: throw std::runtime_error("Unknown accessor type!");
            }

            // TODO: Manually read attributes:
            /*      "JOINTS_0"
                    "NORMAL"
                    "POSITION"
                    "TEXCOORD_0"
                    "WEIGHTS_0"
            */
            // if(attribute.first.compare("POSITION"))
            /*
             * auto byteStide = accessor.ByteStride(gltfModel->bufferViews[accessor.bufferView]);
             * glVertexAttribPointer(???, size, accessor.componentType, accessor.normalized ? GL_TRUE : GL_FALSE, byteStride, BUFFER_OFFSET(accessor.byteOffset);
             * glEnableVertexAttribArray(???)
             *
             */
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

        //TODO: release attributes
        for (auto &attribute : primitive.attributes) {
            //glDisableVertexAttribArray(???)
        }
    }
}