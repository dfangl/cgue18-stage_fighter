//
// Created by raphael on 13.03.18.
//

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Texture.h"

Texture::Texture(const std::string &path) : Logger("Texture") {
    this->textureID = 0;

    glGenTextures(1, &this->textureID);
    glBindTexture(GL_TEXTURE_2D, this->textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    logger->info("Loading Texture from {}", path);

    int nrChannels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data != nullptr) {
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     nrChannels == 4 ? GL_SRGB_ALPHA : GL_SRGB,
                     width,
                     height,
                     0,
                     nrChannels == 4 ? GL_RGBA : GL_RGB,
                     GL_UNSIGNED_BYTE,
                     data
        );
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        logger->error("Failed to load texture: {}", path);
        throw std::runtime_error("Failed to load texture! (" + path + ")");
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}

Texture::Texture(const tinygltf::Image &image, const tinygltf::Sampler &sampler) {
    this->textureID = 0;

    glGenTextures(1, &this->textureID);
    glBindTexture(GL_TEXTURE_2D, this->textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampler.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampler.wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler.magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler.magFilter);

    logger->info("Loading Texture from gltf model: {}", image.name);
    GLuint imgType = sampler.name.c_str()[0] == '0' ? GL_RGB : GL_RGBA;

    this->width = image.width;
    this->height = image.height;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, image.width, image.height, 0, imgType, GL_UNSIGNED_BYTE,
                 reinterpret_cast<const void *>(&image.image.at(0)));

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(GLenum texUnit) {
    glActiveTexture(texUnit);
    glBindTexture(GL_TEXTURE_2D, this->textureID);
}

Texture::~Texture() {
    glDeleteTextures(1, &this->textureID);
}

Texture::Texture(const std::vector<unsigned char> imageData, int width, int height, bool rgba) {
    this->textureID = 0;

    glGenTextures(1, &this->textureID);
    glBindTexture(GL_TEXTURE_2D, this->textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    logger->debug("Create Texture from data pointer {}", (void*)imageData.data());

    this->width = width;
    this->height = height;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, rgba ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, imageData.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

float Texture::pixelHeightToNormal(int px) {
    return (float)px / (float)height;
}

float Texture::pixelWidthToNormal(int px) {
    return (float)px / (float)width;
}
