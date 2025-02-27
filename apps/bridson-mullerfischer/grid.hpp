#pragma once

#pragma once

#include "math/vector.hpp"
#include "util/common.hpp"
#include "util/format.hpp"

class Grid {
public:
    Grid(const Size rows, const Size cols, const Vector2D& cell_center,
         const f32 cell_size);
    Grid(const Size rows, const Size cols, const f32 cell_size);
    Grid(const Grid& other);

    Grid& operator=(const Grid& other);

    ~Grid();

    Size cols() const;
    f32 width() const;

    Size rows() const;
    f32 height() const;

    f32 cellSize() const;

    f32* data();

    f32 operator()(const Index x, const Index y) const;
    f32& operator()(const Index x, const Index y);

    f32 interp(const Vector2D& pos) const;

    Vector2u cell(const Vector2D& pos) const;
    Vector2D toGridSpace(const Vector2D& pos) const;

    void fill(const f32 value);

private:
    /// @brief Number of rows in the grid.
    Size mRows;

    /// @brief Number of columns in the grid.
    Size mCols;

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
                if (y * grid.cols() + x < grid.rows() * grid.cols() - 1)
                    sb.putSafe(',');
            }
            if (y < grid.rows() - 1)
                sb.putSafe('\n');
            else
                sb.putSafe(']');
        }
    }
};
