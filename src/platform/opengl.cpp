#include "opengl.hpp"

void glep_internal(const char* file, const int line) {
    GLenum error;
    StringBuffer log;
    while ((error = glGetError()) != GL_NO_ERROR) {
        if (!log.isEmpty())
            log.append(" | ");

        switch (error) {
            // clang-format off
            case GL_NO_ERROR: log.append("NO_ERROR"); break;
            case GL_INVALID_ENUM: log.append("INVALID_ENUM"); break;
            case GL_INVALID_VALUE: log.append("INVALID_VALUE"); break;
            case GL_INVALID_OPERATION: log.append("INVALID_OPERATION"); break;
            case GL_OUT_OF_MEMORY: log.append("OUT_OF_MEMORY"); break;
            // clang-format on
        }
    }

    if (!log.isEmpty())
        eprintln("OpenGL error at {}:{} : {}", file, line, log.str());
}
