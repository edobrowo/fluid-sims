#pragma once

#include "math/vector.hpp"
#include "util/common.hpp"
#include "util/format.hpp"

class Grid {
public:
    Grid(const i32 nx, const i32 ny, const Vector2D& cell_center,
         const f32 cell_size);
    Grid(const i32 nx, const i32 ny, const f32 cell_size);
    Grid(const Grid& other);

    Grid& operator=(const Grid& other);

    ~Grid();

    /// @brief Number of columns in the grid.
    i32 nx() const;

    /// @brief Number of rows in the grid.
    i32 ny() const;

    /// @brief Number of cells in the grid. Equal to nx() * ny().
    i32 size() const;

    /// @brief Size of a cell in world space.
    f32 cellSize() const;

    /// @brief Center of a cell normalized along [0, 1].
    Vector2D cellCenter() const;

    /// @brief Width of the grid in world space. Equal to nx() * cellSize().
    f32 width() const;

    /// @brief Height of the grid in world space. Equal to ny() * cellSize().
    f32 height() const;

    /// @brief Retrives the value at cell indices (ix, iy).
    f32 operator()(const i32 ix, const i32 iy) const;

    /// @brief Retrives a reference to the value at cell indices (ix, iy).
    f32& operator()(const i32 ix, const i32 iy);

    /// @brief Finds the grid value at the given gridspace position. Uses
    /// bilinear interpolation. Performs extrapolation by clamping; that is, by
    /// finding the closest point on the boundary of the grid and interpolating
    /// with the corresponding cell.
    /// @param pos Worldspace position.
    /// @return Computed value at the worldspace position.
    f32 interp(const Vector2D& grid_pos) const;

    /// @brief Advects the grid through the specified velocity field
    ///  for the time interval given by mTimestep, producing a new grid.
    /// @param u Velocity field along the x axis.
    /// @param v Velocity field along the y axis.
    /// @return Grid at the updated state.
    Grid advect(const Grid& u, const Grid& v, const f32 dt);

    /// @brief Determines the initial position of an imaginary particle with
    /// current position pos integrated back in time by dt.
    Vector2D backtrace(const Vector2D& world_pos, const Grid& u, const Grid& v,
                       const f32 dt) const;

    /// @brief Integrates back in time using Euler.
    Vector2D euler(const Vector2D& world_pos, const Grid& u, const Grid& v,
                   const f32 dt) const;

    /// @brief Integrates back in time with 2nd order Runge-Kutta.
    Vector2D RK2(const Vector2D& world_pos, const Grid& u, const Grid& v,
                 const f32 dt) const;

    // TODO: RK3

    /// @brief Converts a worldspace position to a normalized gridspace
    /// position. Clamps positions to the grid boundaries.
    Vector2D toGridSpace(const Vector2D& world_pos) const;

    /// @brief Converts a gridspace position to a normalized worldspace
    /// position.
    Vector2D toWorldSpace(const Vector2D& grid_pos) const;

    /// @brief Max value in the grid.
    f32 max() const;

    /// @brief Minimum value in the grid.
    f32 min() const;

    /// @brief Fill the grid with a constant value.
    /// @param value Fill value.
    void fill(const f32 value);

    /// @brief Add a constant value to a rectangular region in the grid.
    /// @param world_pos Position of the region.
    /// @param size Size of the region.
    /// @param value Fill value.
    void add(const Vector2D& world_pos, const Vector2D& size, const f32 value);

    /// @brief Retrieve a pointer to the internal buffer.
    f32* data();

private:
    /// @brief Factor to multiple mCellSize for clamping worldspace position to
    /// cell coordinates.
    const f32 cBoundsFactor = 1.001f;

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
                if (iy * grid.nx() + ix < grid.size() - 1)
                    sb.putSafe(',');
            }
            if (iy < grid.ny() - 1)
                sb.putSafe('\n');
            else
                sb.putSafe(']');
        }
    }
};
