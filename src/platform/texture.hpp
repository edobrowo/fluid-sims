#pragma once

#include "opengl.hpp"
#include "util/common.hpp"

class Texture {
public:
    Texture();
    Texture(const Texture& other) = delete;
    ~Texture();

    void bind(const u32 index);
    void image(const u32 width, const u32 height, const GLubyte* data);

private:
    u32 mWidth;
    u32 mHeight;
    GLuint mID;
};
