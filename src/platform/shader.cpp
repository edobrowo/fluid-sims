#include "platform/shader.hpp"

using namespace gl;

ShaderProgram::ShaderProgram() {
    mHandle = glCreateProgram();
    glep();
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(mHandle);
    glep();
}

GLuint ShaderProgram::handle() const {
    return mHandle;
}

void ShaderProgram::addStage(const ShaderKind stage_kind,
                             const char* shader_source) const {
    const GLuint shader = glCreateShader(GLenum(stage_kind));
    glep();
    if (shader == 0) {
        eprintln("Failed to create shader");
        return;
    }

    glShaderSource(shader, 1, &shader_source, NULL);
    glep();
    glCompileShader(shader);
    glep();

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    glep();
    if (!success) {
        const Size size = 512;
        GLchar info_log[size];
        glGetShaderInfoLog(shader, size, NULL, info_log);
        glep();
        eprintln("Failed to compile vertex shader: {}", info_log);
        return;
    }

    glAttachShader(mHandle, shader);
    glep();
    glDeleteShader(shader);
    glep();
}

void ShaderProgram::build() const {
    glLinkProgram(mHandle);
    glep();
}