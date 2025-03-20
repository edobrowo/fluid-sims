#pragma once

#include "math/vector.hpp"
#include "util/common.hpp"
#include "util/format.hpp"

class Grid {
public:
    Grid(const i32 nx, const i32 ny, const Vector2D& cell_center,
         const f32 cell_size);

    Grid(const Grid& other);

    Grid& operator=(const Grid& other);

    ~Grid();

    /// @brief Retrives the value at cell indices (ix, iy).
    f32 operator()(const i32 ix, const i32 iy) const;

    /// @brief Retrives a reference to the value at cell indices (ix, iy).
    f32& operator()(const i32 ix, const i32 iy);

    /// @brief Number of columns in the grid.
    i32 nx() const;

    /// @brief Number of rows in the grid.
    i32 ny() const;

    /// @brief Number of cells in the grid. Equal to nx() * ny().
    i32 cellCount() const;

    /// @brief Size of a cell in world space.
    f32 cellSize() const;

    /// @brief Center of a cell normalized to [0, 1].
    Vector2D cellCenter() const;

    /// @brief Retrieve a pointer to the internal buffer.
    f32* data();

    /// @brief Converts a worldspace position to a normalized gridspace
    /// position.
    Vector2D toGridSpace(const Vector2D& world_pos) const;

    /// @brief Converts a gridspace position to a normalized worldspace
    /// position.
    Vector2D toWorldSpace(const Vector2D& grid_pos) const;

    /// @brief Advects the grid through the specified velocity field
    ///  for the given time step, producing a new grid.
    /// @param u Velocity field along the x axis.
    /// @param v Velocity field along the y axis.
    /// @return Grid at the updated state.
    Grid advect(const Grid& u, const Grid& v, const f32 dt);

    /// @brief Fill the grid with a constant value.
    /// @param value Fill value.
    void fill(const f32 value);

    /// @brief Add a constant value to a rectangular region in the grid.
    /// @param world_pos Position of the region.
    /// @param size Size of the region.
    /// @param value Fill value.
    void add(const Vector2D& world_pos, const Vector2D& size, const f32 value);

    /// @brief Max value in the grid.
    f32 max() const;

    /// @brief Minimum value in the grid.
    f32 min() const;

private:
    /// @brief Finds the grid value at the given gridspace position using
    /// bipolynomial interpolation. Performs extrapolation by clamping. That is,
    /// by finding the closest point on the boundary of the grid and
    /// interpolating with the corresponding cell.
    /// @param pos Worldspace position.
    /// @return Computed value at the worldspace position.
    f32 interp(const Vector2D& grid_pos) const;

    /// @brief Offset used to clamp gridspace positions to cell coordinates.
    const f32 cGridClampOffset = 1.001f;

    /// @brief Clamps the gridspace coordinates to be within grid boundaries.
    Vector2D clampToGrid(const Vector2D& grid_pos) const;

    /// @brief Determines the initial position of an imaginary particle with
    /// current position pos integrated back in time by dt.
    Vector2D backtrace(const Vector2D& grid_pos, const Grid& u, const Grid& v,
                       const f32 dt) const;

    /// @brief Integrates back in time using Euler.
    Vector2D euler(const Vector2D& grid_pos, const Grid& u, const Grid& v,
                   const f32 dt) const;

    /// @brief Integrates back in time with 2nd order Runge-Kutta.
    Vector2D RK2(const Vector2D& grid_pos, const Grid& u, const Grid& v,
                 const f32 dt) const;

    /// @brief Integrates back in time with 3nd order Runge-Kutta.
    Vector2D RK3(const Vector2D& grid_pos, const Grid& u, const Grid& v,
                 const f32 dt) const;

    /// @brief Linearly interpolates the value at the specified position.
    f32 lerp(const Vector2D& grid_pos) const;

    /// @brief Cubicly interpolates the value at the specified position.
    f32 cerp(const Vector2D& grid_pos) const;

    /// @brief Width of the grid in world space. Equal to nx() * cellSize().
    f32 width() const;

    /// @brief Height of the grid in world space. Equal to ny() * cellSize().
    f32 height() const;

    /// @brief Number of columns in the grid.
    i32 mNx;

    /// @brief Number of rows in the grid.
    i32 mNy;

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
        for (Index iy = 0; iy < grid.ny(); ++iy) {
            for (Index ix = 0; ix < grid.nx(); ++ix) {
                FormatWriter<f32>::write(grid(ix, iy), sb);
                if (iy * grid.nx() + ix < grid.cellCount() - 1)
                    sb.putSafe(',');
            }
            if (iy < grid.ny() - 1)
                sb.putSafe('\n');
            else
                sb.putSafe(']');
        }
    }
};
