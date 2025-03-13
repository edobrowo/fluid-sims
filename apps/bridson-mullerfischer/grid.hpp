#pragma once

#pragma once

#include "math/vector.hpp"
#include "util/common.hpp"
#include "util/format.hpp"

class Grid {
public:
    Grid(const i32 rows, const i32 cols, const Vector2D& cell_center,
         const f32 cell_size);
    Grid(const i32 rows, const i32 cols, const f32 cell_size);
    Grid(const Grid& other);

    Grid& operator=(const Grid& other);

    ~Grid();

    /// @brief Number of columns in the grid.
    i32 cols() const;

    /// @brief Number of rows in the grid.
    i32 rows() const;

    /// @brief Number of cells in the grid. Equal to rows() * cols().
    i32 size() const;

    /// @brief Size of a cell in world space.
    f32 cellSize() const;

    /// @brief Width of the grid in world space. Equal to cols() * cellSize().
    f32 width() const;

    /// @brief Height of the grid in world space. Equal to rows() * cellSize().
    f32 height() const;

    /// @brief Retrives the value at cell indices (x, y).
    f32 operator()(const i32 x, const i32 y) const;

    /// @brief Retrives a mutable reference to the value at cell indices (x, y).
    f32& operator()(const i32 x, const i32 y);

    /// @brief Finds the grid value at the given worldspace position. Uses
    /// bilinear interpolation. Performs extrapolation by clamping. That is,
    /// finding the closest point on the boundary of the grid and interpolating
    /// with the corresponding cell.
    /// @param pos Worldspace position.
    /// @return Computed value at the worldspace position.
    f32 interp(const Vector2D& pos) const;

    /// @brief Determines the worldspace position at the given cell coordinate
    /// index pair.
    Vector2D cellToPosition(const Vector2i& cell) const;

    /// @brief Determines the cell coordinate index pair at the given world
    /// position. Clamps at the boundaries.
    Vector2i positionToCell(const Vector2D& pos) const;

    /// @brief Max value in the grid.
    f32 max() const;

    /// @brief Minimum value in the grid.
    f32 min() const;

    /// @brief Fill the grid with a constant value.
    /// @param value Fill value.
    void fill(const f32 value);

    /// @brief Fill the sub-grid with a constant value.
    /// @param lower Lower index pair.
    /// @param upper Upper index pair.
    /// @param value Fill value.
    void fill(const Vector2i& lower, const Vector2i& upper, const f32 value);

    /// @brief Retrieve a pointer to the internal buffer.
    f32* data();

private:
    /// @brief Factor to multiple mCellSize for clamping worldspace position to
    /// cell coordinates.
    const f32 cBoundsFactor = 1.001f;

    /// @brief Number of rows in the grid.
    i32 mRows;

    /// @brief Number of columns in the grid.
    i32 mCols;

    /// @brief Normalized (on [0, 1]) coordinates of the cell center.
    Vector2D mCellCenter;

    /// @brief Size of a cell.
    f32 mCellSize;

    /// @brief Grid data.
    f32* mData;
};

template <>
struct FormatWriter<Grid> {
    static void write(const Grid& grid, StringBuffer& sb) {
        sb.putSafe('[');
        for (Index y = 0; y < grid.rows(); ++y) {
            for (Index x = 0; x < grid.cols(); ++x) {
                FormatWriter<f32>::write(grid(x, y), sb);
                if (y * grid.cols() + x < grid.size() - 1)
                    sb.putSafe(',');
            }
            if (y < grid.rows() - 1)
                sb.putSafe('\n');
            else
                sb.putSafe(']');
        }
    }
};
