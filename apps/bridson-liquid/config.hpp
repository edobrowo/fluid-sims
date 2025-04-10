#pragma once

#include "util/common.hpp"

struct Config {
    Size rows;
    Size cols;
    f64 cellSize;
    f64 timestep;
    bool saveFrames;

    static Config loadFromJson(const std::string& path);
};
