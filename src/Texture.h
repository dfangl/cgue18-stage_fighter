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
class Texture : public Logger {

protected:
    GLuint textureID;
    Texture(): Logger("Texture") { this->textureID = 0; }

    int height, width;

public:
    /**
     * Creates a Texture from a
     * @param path
     */
    explicit Texture(const std::string &path);

    /**
     * Load the Texture from a Image which is embedded in a gltf Model
     * @param image
     * @param sampler
     */
    Texture(const tinygltf::Image &image, const tinygltf::Sampler &sampler);

    /**
     * Generate a Texture from raw data
     * @param imageData
     * @param height
     * @param width
     * @param rgba
     */
    Texture(const std::vector<unsigned char> imageData, int width, int height, bool rgba = false);

    ~Texture();

    virtual void bind(GLenum texUnit);

    float pixelHeightToNormal(int px);
    float pixelWidthToNormal(int px);

};


#endif //STAGE_FIGTHER_TEXTURE_H
