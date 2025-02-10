#pragma once

#include "util/common.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Image {
    Image(const char* path);
    ~Image();

    unsigned char* data;
    i32 width;
    i32 height;
    i32 channelCount;
};

Image::Image(const char* path) {
    data = stbi_load(path, &width, &height, &channelCount, 0);
}

Image::~Image() {
    stbi_image_free(data);
}
