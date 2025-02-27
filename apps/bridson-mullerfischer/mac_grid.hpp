#pragma once

#include "grid.hpp"
#include "math/vector.hpp"
#include "util/common.hpp"

class MACGrid {
public:
    MACGrid(const Size rows, const Size cols, const f32 cell_size);
    ~MACGrid() = default;

    Vector2D velocity(const Vector2D& pos) const;

private:
    /// @brief Velocity x-component.
    Grid mU;

    /// @brief Velocity y-component.
    Grid mV;

    /// @brief Pressure.
    Grid mP;

    /// @brief Density.
    Grid mD;

    /// @brief Temperature.
    Grid mT;
};
