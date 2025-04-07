#pragma once

#include <array>

#include "opengl.hpp"
#include "uniforms.hpp"
#include "util/common.hpp"

namespace gl {

enum class ShaderKind {
    Vertex = GL_VERTEX_SHADER,
    TessControl = GL_TESS_CONTROL_SHADER,
    TessEvaluation = GL_TESS_EVALUATION_SHADER,
    Geometry = GL_GEOMETRY_SHADER,
    Fragment = GL_FRAGMENT_SHADER,
};

class ShaderProgram {
public:
    using Handle = GLuint;

    ShaderProgram();
    ~ShaderProgram();

    Handle handle() const;

    void addStage(const ShaderKind stage_kind, const char* shader_source) const;
    void build() const;
    void use() const;

    template <typename T>
    std::optional<T> uniform(const char* name) const;
    template <typename T, u32 length>
    std::optional<std::array<T, length>> uniform(const char* name) const;

    template <typename T>
    void setUniform(const char* name, const T c) const;
    template <typename T, u32 length>
    void setUniform(const char* name, const std::array<T, length> value) const;

private:
    Handle mHandle;
};

template <typename T, u32 length>
std::optional<std::array<T, length>> ShaderProgram::uniform(
    const char* name) const {
    const GLint loc = glGetUniformLocation(mHandle, name);
    glep();

    if (loc == -1)
        return std::nullopt;

    return UniformReader<T, length>::get(mHandle, loc);
}

template <typename T>
std::optional<T> ShaderProgram::uniform(const char* name) const {
    const GLint loc = glGetUniformLocation(mHandle, name);
    glep();

    if (loc == -1)
        return std::nullopt;

    std::optional<std::array<T, 1>> result =
        UniformReader<T, 1>::get(mHandle, loc);
    if (result.has_value())
        return result.value()[0];
    return std::nullopt;
}

template <typename T>
void ShaderProgram::setUniform(const char* name, const T value) const {
    const GLint loc = glGetUniformLocation(mHandle, name);
    glep();

    if (loc == -1)
        return;

    UniformWriter<T>::set(mHandle, loc, value);
}

template <typename T, u32 length>
void ShaderProgram::setUniform(const char* name,
                               const std::array<T, length> value) const {
    const GLint loc = glGetUniformLocation(mHandle, name);
    glep();

    if (loc == -1)
        return;

    UniformVectorWriter<T, length>::set(mHandle, loc, value);
}

}
