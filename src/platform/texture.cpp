#include "texture.hpp"

Texture::Texture() {
    glGenTextures(1, &mID);
}

Texture::~Texture() {
    glDeleteTextures(1, &mID);
}

void Texture::bind(const u32 index) {
    glActiveTexture(GL_TEXTURE0 + index);
    glep();
    glBindTexture(GL_TEXTURE_2D, mID);
    glep();
}

void Texture::image(const u32 width, const u32 height, const GLubyte* data) {
    mWidth = width;
    mHeight = height;

    glBindTexture(GL_TEXTURE_2D, mID);
    glep();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glep();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glep();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glep();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glep();

    glBindTexture(GL_TEXTURE_2D, 0);
    glep();
}
