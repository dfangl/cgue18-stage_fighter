//
// Created by raphael on 13.03.18.
//

#ifndef STAGE_FIGTHER_TEXTURE_H
#define STAGE_FIGTHER_TEXTURE_H

#include <string>

#include <glad/glad.h>

#include <tiny_gltf.h>

#include "helper/Logger.h"

/**
 * This class represents a Texture which is Stored on the GPU Memory, with
 * bind(...) the texture can be bound to a texture unit for use
 *
 * sbt_image is used to load the Texture from Disk
 */
class Texture : Logger {

private:
    GLuint textureID;

public:
    /**
     * Creates a Texture from a
     * @param path
     */
    explicit Texture(const std::string &path);
    /**
     *
     * @param image
     * @param sampler
     */
    Texture(const tinygltf::Image &image, const tinygltf::Sampler &sampler);

    ~Texture();

    void bind(GLenum texUnit);

};


#endif //STAGE_FIGTHER_TEXTURE_H
