//
// Created by raphael on 14.05.18.
//

#include <stb_image.h>

#include "CubemapTexture.h"

const std::string CubemapTexture::locationNames[CUBEMAP_FACES] = {
        "Right", "Left", "Top", "Bottom", "Back", "Front"
};

CubemapTexture::CubemapTexture(const std::string &path, const std::string &extension) : Texture() {

    glGenTextures(1, &this->textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureID);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    logger->debug("Loading Cubemap texture from {}", path);

    for (int i=0; i<CUBEMAP_FACES; i++) {
        std::string facePath = path + std::string("_") + locationNames[i] + extension;

        int width, height, nrChannels;
        unsigned char *data = stbi_load(facePath.c_str(), &width, &height, &nrChannels, 0);

        if (data != nullptr) {
            glTexImage2D(
                    static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i),
                    0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
        } else {
            logger->error("Failed to load texture: {}", facePath);
            throw std::runtime_error("Failed to load texture!");
        }

        stbi_image_free(data);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CubemapTexture::bind(GLenum texUnit) {
    glActiveTexture(texUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureID);
}
