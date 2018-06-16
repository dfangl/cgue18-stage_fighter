//
// Created by raphael on 07.05.18.
//

#include "ImageGenerator.h"

#include <climits>

#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>

#include <spdlog/spdlog.h>

std::vector<unsigned char> ImageGenerator::marble(unsigned int width, unsigned int height, glm::vec4 color, bool rgba) {
    std::vector<unsigned char> data;
    data.reserve(height * width * (rgba ? 4 : 3));

    // TODO: make one loop and use OpenMP to speed things up
    for (unsigned int x=0; x < width; x++)
        for (unsigned int y=0; y < height; y++) {
            float nx = (float)x / width - 0.5f;
            float ny = (float)y / height - 0.5f;
            float nz = 15.296f;


            auto noise =
                    1    * stb_perlin_ridge_noise3(2 * nx, 2 * ny, nz, 2.0, 0.5, 1.0, 24, 2, 2, 64) +
                    .5   * stb_perlin_ridge_noise3(4 * nx, 4 * ny, nz, 2.0, 0.5, 1.0, 24, 4, 4, 64) +
                    .5   * stb_perlin_ridge_noise3(6 * nx, 6 * ny, nz, 2.0, 0.5, 1.0, 24, 6, 6, 64) +
                    .125 * stb_perlin_ridge_noise3(16 * nx, 8 * ny, nz, 2.0, 0.5, 1.0, 24, 16, 8, 32) +
                    .125 * stb_perlin_ridge_noise3(4 * nx, 8 * ny, nz, 2.0, 0.5, 0.9, 24, 4, 8, 64);
            auto value = 1.9f * (glm::pow(noise / 2.5f, 3.23463) + 0.5) - 0.5;
            value = glm::pow(value, 2.0);

            auto r = static_cast<unsigned char>(UCHAR_MAX - value * color.r * UCHAR_MAX);
            auto g = static_cast<unsigned char>(UCHAR_MAX - value * color.g * UCHAR_MAX);
            auto b = static_cast<unsigned char>(UCHAR_MAX - value * color.b * UCHAR_MAX);

            data.push_back(r);
            data.push_back(g);
            data.push_back(b);

            if (rgba) data.push_back(static_cast<unsigned char &&>(color.w * UCHAR_MAX));
        }

    return data;
}
