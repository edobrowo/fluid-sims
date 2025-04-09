#pragma once

#include "math/vector.hpp"
#include "util/common.hpp"
#include "util/format.hpp"

class Grid {
public:
    Grid(const i32 nx,
         const i32 ny,
         const Vector2D& cell_center,
         const f64 cell_size);

    Grid(const Grid& other);

    Grid& operator=(const Grid& other);

    ~Grid();

    /// @brief Retrives the value at cell indices (i, j).
    f64 operator()(const i32 i, const i32 j) const;

    /// @brief Retrives a reference to the value at cell indices (i, j).
    f64& operator()(const i32 i, const i32 j);

    /// @brief Number of columns in the grid.
    i32 nx() const;

    /// @brief Number of rows in the grid.
    i32 ny() const;

    /// @brief Number of cells in the grid. Equal to nx() * ny().
    i32 cellCount() const;

    /// @brief Size of a cell in world space.
    f64 cellSize() const;

    /// @brief Center of a cell normalized to [0, 1].
    Vector2D cellCenter() const;

    /// @brief Retrieve a pointer to the internal buffer.
    f64* data();

    /// @brief Converts a worldspace position to a normalized gridspace
    /// position.
    Vector2D toGridSpace(const Vector2D& world_pos) const;

    /// @brief Converts a gridspace position to a normalized worldspace
    /// position.
    Vector2D toWorldSpace(const Vector2D& grid_pos) const;

    /// @brief Finds the grid value at the given gridspace position using
    /// bipolynomial interpolation. Performs extrapolation by clamping. That is,
    /// by finding the closest point on the boundary of the grid and
    /// interpolating with the corresponding cell.
    /// @param pos Worldspace position.
    /// @return Computed value at the worldspace position.
    f64 interp(const Vector2D& grid_pos) const;

    /// @brief Fill the grid with a constant value.
    /// @param value Fill value.
    void fill(const f64 value);

    /// @brief Add a constant value to a rectangular region in the grid.
    /// @param world_pos Position of the region.
    /// @param size Size of the region.
    /// @param value Fill value.
    void add(const Vector2D& world_pos, const Vector2D& size, const f64 value);

    /// @brief Max value in the grid.
    f64 max() const;

    /// @brief Minimum value in the grid.
    f64 min() const;

private:
    /// @brief Offset used to clamp gridspace positions to cell coordinates.
    const f64 cGridClampOffset = 1.001;

    /// @brief Clamps the gridspace coordinates to be within grid boundaries.
    Vector2D clampToGrid(const Vector2D& grid_pos) const;

    /// @brief Linearly interpolates the value at the specified position.
    f64 lerp(const Vector2D& grid_pos) const;

    /// @brief Cubicly interpolates the value at the specified position.
    f64 cerp(const Vector2D& grid_pos) const;

    /// @brief Width of the grid in world space. Equal to nx() * cellSize().
    f64 width() const;

    /// @brief Height of the grid in world space. Equal to ny() * cellSize().
    f64 height() const;

    /// @brief Number of columns in the grid.
    i32 mNx;

    /// @brief Number of rows in the grid.
    i32 mNy;

    /// @brief Normalized (on [0, 1]) coordinates of the cell center.
    Vector2D mCellCenter;

    /// @brief Size of a cell.
    f64 mCellSize;

    /// @brief Grid data.
    f64* mData;
};

template <>
struct FormatWriter<Grid> {
    static void write(const Grid& grid, StringBuffer& sb) {
        sb.putSafe('[');
        for (Index j = 0; j < grid.ny(); ++j) {
            for (Index i = 0; i < grid.nx(); ++i) {
                FormatWriter<f64>::write(grid(i, j), sb);
                if (j * grid.nx() + i < grid.cellCount() - 1)
                    sb.putSafe(',');
            }
            if (j < grid.ny() - 1)
                sb.putSafe('\n');
            else
                sb.putSafe(']');
        }
    }
};
