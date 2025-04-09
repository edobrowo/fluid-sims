#pragma once

#include "grid.hpp"
#include "math/vector.hpp"
#include "util/common.hpp"

class MACGrid {
public:
    MACGrid(const i32 rows, const i32 cols, const f64 cell_size);
    ~MACGrid() = default;

    /// @brief Velocity x-component.
    Grid u;

    /// @brief Velocity y-component.
    Grid v;

    /// @brief Pressure.
    Grid p;

    /// @brief Density.
    Grid d;

    /// @brief Number of columns in the MAC grid.
    i32 nx() const;

    /// @brief Number of rows in the MAC grid.
    i32 ny() const;

    /// @brief Number of cells in the grid. Equal to nx() * ny().
    i32 cellCount() const;

    /// @brief Size of a cell in world space.
    f64 cellSize() const;

private:
    /// @brief Width of the MAC grid in world space. Equal to nx() * cellSize().
    f64 width() const;

    /// @brief Height of the MAC grid in world space. Equal to ny() *
    /// cellSize().
    f64 height() const;

    /// @brief Uses the CFL heuristic to determine a large timestep.
    f64 cflTimestep() const;

    i32 mNx;
    i32 mNy;

    f64 mCellSize;
};
