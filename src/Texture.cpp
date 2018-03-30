//
// Created by raphael on 13.03.18.
//

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Texture.h"

Texture::Texture(const std::string &path) : Logger("Texture") {
    glGenTextures(1, &this->textureID);
    glBindTexture(GL_TEXTURE_2D, this->textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data != nullptr) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        logger->error("Failed to load texture: {}", path);
        throw std::runtime_error("Failed to load texture!");
    }

    stbi_image_free(data);
}

Texture::Texture(const tinygltf::Image &image, const tinygltf::Sampler &sampler) {
    glGenTextures(1, &this->textureID);
    glBindTexture(GL_TEXTURE_2D, this->textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampler.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampler.wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler.magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler.magFilter);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 reinterpret_cast<const void *>(&image.image.at(0)));

    glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::bind(GLenum texUnit) {
    glActiveTexture(texUnit);
    glBindTexture(GL_TEXTURE_2D, this->textureID);
}

Texture::~Texture() {
    glDeleteTextures(1, &this->textureID);
}


