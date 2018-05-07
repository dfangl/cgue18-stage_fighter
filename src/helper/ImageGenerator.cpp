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

    for (int x=0; x < width; x++)
        for (int y=0; y < height; y++) {
            auto noise = stb_perlin_noise3(x / 45.94f, y / 45.94f, 1.15f, 256, 256, 12);
            auto value = (1 + glm::sin( (x + noise / 2 ) * 50) ) / 2;

            //spdlog::get("console")->info("({},{}) noise: {}, value: {}", x,y, noise, value);

            data.push_back(static_cast<unsigned char &&>(value * color.x * UCHAR_MAX));
            data.push_back(static_cast<unsigned char &&>(value * color.y * UCHAR_MAX));
            data.push_back(static_cast<unsigned char &&>(value * color.z * UCHAR_MAX));

            if (rgba) data.push_back(static_cast<unsigned char &&>(color.w * UCHAR_MAX));
        }

    return data;
}
