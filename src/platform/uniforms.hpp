#pragma once

#include "math/matrix.hpp"
#include "math/vector.hpp"
#include "opengl.hpp"
#include "util/common.hpp"
#include "util/format.hpp"
#include "util/stringbuffer.hpp"

namespace {

using Handle = GLuint;

}

namespace gl {

template <typename T, u32 length>
struct UniformReader {
    using Value = std::array<T, length>;
    using Result = std::optional<Value>;

    static Result get(const Handle handle, const GLint loc) {
        return std::nullopt;
    }
};

template <typename T>
struct UniformWriter {
    using Value = T;

    static void set(const Handle handle, const GLint loc, Value value) {
        return;
    }
};

template <typename T, u32 length>
struct UniformVectorWriter {
    using Value = std::array<T, length>;

    static void set(const Handle handle, const GLint loc, Value value) {
        return;
    }
};

template <u32 length>
struct UniformReader<bool, length> {
    using Value = std::array<bool, length>;
    using Result = std::optional<Value>;

    static Result get(const Handle handle, const GLint loc) {
        GLfloat gl_value[length];
        glGetUniformiv(handle, loc, gl_value);
        glep();

        Value value;
        for (Index i = 0; i < length; ++i)
            value[i] = static_cast<bool>(gl_value[i]);

        return value;
    }
};

template <u32 length>
struct UniformReader<i32, length> {
    using Value = std::array<i32, length>;
    using Result = std::optional<Value>;

    static Result get(const Handle handle, const GLint loc) {
        GLfloat gl_value[length];
        glGetUniformiv(handle, loc, gl_value);
        glep();

        Value value;
        for (Index i = 0; i < length; ++i)
            value[i] = static_cast<i32>(gl_value[i]);

        return value;
    }
};

template <u32 length>
struct UniformReader<u32, length> {
    using Value = std::array<u32, length>;
    using Result = std::optional<Value>;

    static Result get(const Handle handle, const GLint loc) {
        GLfloat gl_value[length];
        glGetUniformuiv(handle, loc, gl_value);
        glep();

        Value value;
        for (Index i = 0; i < length; ++i)
            value[i] = static_cast<u32>(gl_value[i]);

        return value;
    }
};

template <u32 length>
struct UniformReader<f32, length> {
    using Value = std::array<f32, length>;
    using Result = std::optional<Value>;

    static Result get(const Handle handle, const GLint loc) {
        GLfloat gl_value[length];
        glGetUniformfv(handle, loc, gl_value);
        glep();

        Value value;
        for (Index i = 0; i < length; ++i)
            value[i] = static_cast<f32>(gl_value[i]);

        return value;
    }
};

template <u32 length>
struct UniformReader<f64, length> {
    using Value = std::array<f64, length>;
    using Result = std::optional<Value>;

    static Result get(const Handle handle, const GLint loc) {
        GLdouble gl_value[length];
        glGetUniformfv(handle, loc, gl_value);
        glep();

        Value value;
        for (Index i = 0; i < length; ++i)
            value[i] = static_cast<f64>(gl_value[i]);

        return value;
    }
};

template <>
struct UniformWriter<bool> {
    using Value = bool;

    static void set(const Handle handle, const GLint loc, Value value) {
        glUniform1i(loc, static_cast<GLint>(value));
        glep();
    }
};

template <>
struct UniformWriter<i32> {
    using Value = i32;

    static void set(const Handle handle, const GLint loc, Value value) {
        glUniform1i(loc, static_cast<GLint>(value));
        glep();
    }
};

template <>
struct UniformWriter<u32> {
    using Value = u32;

    static void set(const Handle handle, const GLint loc, Value value) {
        glUniform1ui(loc, static_cast<GLuint>(value));
        glep();
    }
};

template <>
struct UniformWriter<f32> {
    using Value = f32;

    static void set(const Handle handle, const GLint loc, Value value) {
        glUniform1f(loc, static_cast<GLfloat>(value));
        glep();
    }
};

template <>
struct UniformWriter<f64> {
    using Value = f64;

    static void set(const Handle handle, const GLint loc, Value value) {
        glUniform1d(loc, static_cast<GLdouble>(value));
        glep();
    }
};

template <u32 dim>
struct UniformWriter<Vector<f32, dim>> {
    using Value = Vector<f32, dim>;

    static void set(const Handle handle, const GLint loc, Value value) {
        if constexpr (dim == 1)
            glUniform1f(loc, static_cast<GLfloat>(value[0]));
        if constexpr (dim == 2)
            glUniform2f(loc,
                        static_cast<GLfloat>(value[0]),
                        static_cast<GLfloat>(value[1]));
        if constexpr (dim == 3)
            glUniform3f(loc,
                        static_cast<GLfloat>(value[0]),
                        static_cast<GLfloat>(value[1]),
                        static_cast<GLfloat>(value[2]));
        if constexpr (dim == 4)
            glUniform4f(loc,
                        static_cast<GLfloat>(value[0]),
                        static_cast<GLfloat>(value[1]),
                        static_cast<GLfloat>(value[2]),
                        static_cast<GLfloat>(value[3]));
        else
            return;
        glep();
    }
};

template <u32 dim>
struct UniformWriter<Vector<f64, dim>> {
    using Value = Vector<f64, dim>;

    static void set(const Handle handle, const GLint loc, Value value) {
        if constexpr (dim == 1)
            glUniform1d(loc, static_cast<GLdouble>(value[0]));
        if constexpr (dim == 2)
            glUniform2d(loc,
                        static_cast<GLdouble>(value[0]),
                        static_cast<GLdouble>(value[1]));
        if constexpr (dim == 3)
            glUniform3d(loc,
                        static_cast<GLdouble>(value[0]),
                        static_cast<GLdouble>(value[1]),
                        static_cast<GLdouble>(value[2]));
        if constexpr (dim == 4)
            glUniform4d(loc,
                        static_cast<GLdouble>(value[0]),
                        static_cast<GLdouble>(value[1]),
                        static_cast<GLdouble>(value[2]),
                        static_cast<GLdouble>(value[3]));
        else
            return;
        glep();
    }
};

template <u32 p, u32 q>
struct UniformWriter<Matrix<f32, p, q>> {
    using Value = Matrix<f32, p, q>;

    static void set(const Handle handle, const GLint loc, Value value) {
        const GLfloat* value_ptr = static_cast<const GLfloat*>(value.data());
        if constexpr (p == 2 && q == 2)
            glUniformMatrix2fv(loc, 1, GL_TRUE, value_ptr);
        else if constexpr (p == 2 && q == 3)
            glUniformMatrix2x3fv(loc, 1, GL_TRUE, value_ptr);
        else if constexpr (p == 2 && q == 4)
            glUniformMatrix2x4fv(loc, 1, GL_TRUE, value_ptr);
        else if constexpr (p == 3 && q == 2)
            glUniformMatrix3x2fv(loc, 1, GL_TRUE, value_ptr);
        else if constexpr (p == 3 && q == 3)
            glUniformMatrix3fv(loc, 1, GL_TRUE, value_ptr);
        else if constexpr (p == 3 && q == 4)
            glUniformMatrix3x4fv(loc, 1, GL_TRUE, value_ptr);
        else if constexpr (p == 4 && q == 2)
            glUniformMatrix4x2fv(loc, 1, GL_TRUE, value_ptr);
        else if constexpr (p == 4 && q == 3)
            glUniformMatrix4x3fv(loc, 1, GL_TRUE, value_ptr);
        else if constexpr (p == 4 && q == 4)
            glUniformMatrix4fv(loc, 1, GL_TRUE, value_ptr);
        else
            return;
        glep();
    }
};

template <u32 p, u32 q>
struct UniformWriter<Matrix<f64, p, q>> {
    using Value = Matrix<f64, p, q>;

    static void set(const Handle handle, const GLint loc, Value value) {
        const GLdouble* value_ptr = static_cast<const GLdouble*>(value.data());
        if constexpr (p == 2 && q == 2)
            glUniformMatrix2dv(loc, 1, GL_TRUE, value_ptr);
        else if constexpr (p == 2 && q == 3)
            glUniformMatrix2x3dv(loc, 1, GL_TRUE, value_ptr);
        else if constexpr (p == 2 && q == 4)
            glUniformMatrix2x4dv(loc, 1, GL_TRUE, value_ptr);
        else if constexpr (p == 3 && q == 2)
            glUniformMatrix3x2dv(loc, 1, GL_TRUE, value_ptr);
        else if constexpr (p == 3 && q == 3)
            glUniformMatrix3dv(loc, 1, GL_TRUE, value_ptr);
        else if constexpr (p == 3 && q == 4)
            glUniformMatrix3x4dv(loc, 1, GL_TRUE, value_ptr);
        else if constexpr (p == 4 && q == 2)
            glUniformMatrix4x2dv(loc, 1, GL_TRUE, value_ptr);
        else if constexpr (p == 4 && q == 3)
            glUniformMatrix4x3dv(loc, 1, GL_TRUE, value_ptr);
        else if constexpr (p == 4 && q == 4)
            glUniformMatrix4dv(loc, 1, GL_TRUE, value_ptr);
        else
            return;
        glep();
    }
};

template <u32 length>
struct UniformVectorWriter<bool, length> {
    using Value = std::array<bool, length>;

    static void set(const Handle handle, const GLint loc, Value value) {
        const GLint* value_ptr = static_cast<const GLint*>(value.data());
        glUniform1iv(loc, length, value_ptr);
        glep();
    }
};

template <u32 length>
struct UniformVectorWriter<i32, length> {
    using Value = std::array<i32, length>;

    static void set(const Handle handle, const GLint loc, Value value) {
        const GLint* value_ptr = static_cast<const GLint*>(value.data());
        glUniform1iv(loc, length, value_ptr);
        glep();
    }
};

template <u32 length>
struct UniformVectorWriter<u32, length> {
    using Value = std::array<u32, length>;

    static void set(const Handle handle, const GLint loc, Value value) {
        const GLuint* value_ptr = static_cast<const GLuint*>(value.data());
        glUniform1uiv(loc, length, value_ptr);
        glep();
    }
};

template <u32 length>
struct UniformVectorWriter<f32, length> {
    using Value = std::array<f32, length>;

    static void set(const Handle handle, const GLint loc, Value value) {
        const GLfloat* value_ptr = static_cast<const GLfloat*>(value.data());
        glUniform1fv(loc, length, value_ptr);
        glep();
    }
};

template <u32 length>
struct UniformVectorWriter<f64, length> {
    using Value = std::array<f64, length>;

    static void set(const Handle handle, const GLint loc, Value value) {
        const GLdouble* value_ptr = static_cast<const GLdouble*>(value.data());
        glUniform1dv(loc, length, value_ptr);
        glep();
    }
};

template <u32 dim, u32 length>
struct UniformVectorWriter<Vector<f32, dim>, length> {
    using Value = std::array<Vector<f32, dim>, length>;

    static void set(const Handle handle, const GLint loc, Value value) {
        const GLfloat* value_ptr = static_cast<const GLfloat*>(value.data());
        if constexpr (dim == 1)
            glUniform1fv(loc, length, value_ptr);
        else if constexpr (dim == 2)
            glUniform2fv(loc, length, value_ptr);
        else if constexpr (dim == 3)
            glUniform3fv(loc, length, value_ptr);
        else if constexpr (dim == 4)
            glUniform4fv(loc, length, value_ptr);
        else
            return;
        glep();
    }
};

template <u32 dim, u32 length>
struct UniformVectorWriter<Vector<f64, dim>, length> {
    using Value = std::array<Vector<f64, dim>, length>;

    static void set(const Handle handle, const GLint loc, Value value) {
        const GLdouble* value_ptr = static_cast<const GLdouble*>(value.data());
        if constexpr (dim == 1)
            glUniform1dv(loc, length, value_ptr);
        else if constexpr (dim == 2)
            glUniform2dv(loc, length, value_ptr);
        else if constexpr (dim == 3)
            glUniform3dv(loc, length, value_ptr);
        else if constexpr (dim == 4)
            glUniform4dv(loc, length, value_ptr);
        else
            return;
        glep();
    }
};

template <u32 p, u32 q, u32 length>
struct UniformVectorWriter<Matrix<f32, p, q>, length> {
    using Value = std::array<Matrix<f32, p, q>, length>;

    static void set(const Handle handle, const GLint loc, Value value) {
        const GLfloat* value_ptr = static_cast<const GLfloat*>(value.data());
        if constexpr (p == 2 && q == 2)
            glUniformMatrix2fv(loc, length, GL_TRUE, value_ptr);
        else if constexpr (p == 2 && q == 3)
            glUniformMatrix2x3fv(loc, length, GL_TRUE, value_ptr);
        else if constexpr (p == 2 && q == 4)
            glUniformMatrix2x4fv(loc, length, GL_TRUE, value_ptr);
        else if constexpr (p == 3 && q == 2)
            glUniformMatrix3x2fv(loc, length, GL_TRUE, value_ptr);
        else if constexpr (p == 3 && q == 3)
            glUniformMatrix3fv(loc, length, GL_TRUE, value_ptr);
        else if constexpr (p == 3 && q == 4)
            glUniformMatrix3x4fv(loc, length, GL_TRUE, value_ptr);
        else if constexpr (p == 4 && q == 2)
            glUniformMatrix4x2fv(loc, length, GL_TRUE, value_ptr);
        else if constexpr (p == 4 && q == 3)
            glUniformMatrix4x3fv(loc, length, GL_TRUE, value_ptr);
        else if constexpr (p == 4 && q == 4)
            glUniformMatrix4fv(loc, length, GL_TRUE, value_ptr);
        else
            return;
        glep();
    }
};

template <u32 p, u32 q, u32 length>
struct UniformVectorWriter<Matrix<f64, p, q>, length> {
    using Value = std::array<Matrix<f64, p, q>, length>;

    static void set(const Handle handle, const GLint loc, Value value) {
        const GLdouble* value_ptr = static_cast<const GLdouble*>(value.data());
        if constexpr (p == 2 && q == 2)
            glUniformMatrix2dv(loc, length, GL_TRUE, value_ptr);
        else if constexpr (p == 2 && q == 3)
            glUniformMatrix2x3dv(loc, length, GL_TRUE, value_ptr);
        else if constexpr (p == 2 && q == 4)
            glUniformMatrix2x4dv(loc, length, GL_TRUE, value_ptr);
        else if constexpr (p == 3 && q == 2)
            glUniformMatrix3x2dv(loc, length, GL_TRUE, value_ptr);
        else if constexpr (p == 3 && q == 3)
            glUniformMatrix3dv(loc, length, GL_TRUE, value_ptr);
        else if constexpr (p == 3 && q == 4)
            glUniformMatrix3x4dv(loc, length, GL_TRUE, value_ptr);
        else if constexpr (p == 4 && q == 2)
            glUniformMatrix4x2dv(loc, length, GL_TRUE, value_ptr);
        else if constexpr (p == 4 && q == 3)
            glUniformMatrix4x3dv(loc, length, GL_TRUE, value_ptr);
        else if constexpr (p == 4 && q == 4)
            glUniformMatrix4dv(loc, length, GL_TRUE, value_ptr);
        else
            return;
        glep();
    }
};

}
