#pragma once

#include "config.hpp"
#include "mac_grid.hpp"

class Solver {
public:
    Solver(const Config& config);
    ~Solver() = default;

    void step();

private:
    MACGrid mGrid;
    f32 mTimestep;
};
