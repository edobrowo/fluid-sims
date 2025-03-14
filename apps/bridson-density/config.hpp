#pragma once

#include "util/common.hpp"

struct Config {
    Size rows;
    Size cols;
    f32 cellSize;

    f32 timestep;
    f32 density;
};
