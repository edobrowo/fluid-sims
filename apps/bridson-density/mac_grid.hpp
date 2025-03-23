#pragma once

#include "grid.hpp"
#include "math/vector.hpp"
#include "util/common.hpp"

class MACGrid {
public:
    MACGrid(const i32 rows, const i32 cols, const f32 cell_size);
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
    f32 cellSize() const;

    /// @brief Number of faces holding a value along the X axis in the MAC grid.
    i32 facesX() const;

    /// @brief Number of faces holding a value along the Y axis in the MAC grid.
    i32 facesY() const;

    /// @brief Determines whether the index pair corresponds to a cell within
    /// the (nx(), ny()) bounds of the grid.
    /// @param ix Column index.
    /// @param iy Row index.
    bool isCellInBounds(const i32 ix, const i32 iy) const;

    /// @brief Determines whether the index pair corresponds to a X-axis face
    /// within the (nx(), ny()) bounds of the grid.
    /// @param ix Column index.
    /// @param iy Row index.
    bool isFaceXInBounds(const i32 ix, const i32 iy) const;

    /// @brief Determines whether the index pair corresponds to a Y-axis face
    /// within the (nx(), ny()) bounds of the grid.
    /// @param ix Column index.
    /// @param iy Row index.
    bool isFaceYInBounds(const i32 ix, const i32 iy) const;

private:
    /// @brief Width of the MAC grid in world space. Equal to nx() * cellSize().
    f32 width() const;

    /// @brief Height of the MAC grid in world space. Equal to ny() *
    /// cellSize().
    f32 height() const;

    /// @brief Uses the CFL heuristic to determine a large timestep.
    f32 cflTimestep() const;

    i32 mNx;
    i32 mNy;
    i32 mFacesX;
    i32 mFacesY;

    f32 mCellSize;
};
