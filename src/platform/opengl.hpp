#pragma once

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#undef GLFW_INCLUDE_ES1
#include <GLFW/glfw3.h>

#else  // __APPLE__ undefined
#include <GLFW/glfw3.h>

#endif

#include "util/format.hpp"
#include "util/stringbuffer.hpp"

void glep_internal(const char* file, const int line);

#define glep() glep_internal(__FILE__, __LINE__)
