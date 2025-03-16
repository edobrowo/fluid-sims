#include "grid.hpp"

#include <algorithm>

#include "math/common.hpp"

namespace {}

Grid::Grid(const i32 rows, const i32 cols, const Vector2D& cell_center,
           const f32 cell_size)
    : mNx(cols),
      mNy(rows),
      mCellCenter(cell_center.clamped(0.0f, 1.0f)),
      mCellSize(cell_size) {
    assertm(mNy > 0, "number of rows must be positive");
    assertm(mNx > 0, "number of cols must be positive");
    assertm(mCellSize != 0.0f, "cell size must be nonzero");

    mData = new f32[mNx * mNy];
}

Grid::Grid(const Grid& other)
    : mNx(other.mNx),
      mNy(other.mNy),
      mCellCenter(other.mCellCenter),
      mCellSize(other.mCellSize),
      mData(new f32[other.mNx * other.mNy]) {
    std::copy(other.mData, other.mData + (mNx * mNy), mData);
}

Grid& Grid::operator=(const Grid& other) {
    mNx = other.mNx;
    mNy = other.mNy;
    mCellCenter = other.mCellCenter;
    mCellSize = other.mCellSize;
    std::copy(other.mData, other.mData + (mNx * mNy), mData);
    return *this;
}

Grid::~Grid() {
    delete[] mData;
}

f32 Grid::operator()(const i32 ix, const i32 iy) const {
    assertm(ix >= 0, "x out of bounds");
    assertm(ix < mNx, "x out of bounds");
    assertm(iy >= 0, "y out of bounds");
    assertm(iy < mNy, "y out of bounds");

    return mData[iy * mNx + ix];
}

f32& Grid::operator()(const i32 ix, const i32 iy) {
    assertm(ix >= 0, "x out of bounds");
    assertm(ix < mNx, "x out of bounds");
    assertm(iy >= 0, "y out of bounds");
    assertm(iy < mNy, "y out of bounds");

    return mData[iy * mNx + ix];
}

i32 Grid::nx() const {
    return mNx;
}

i32 Grid::ny() const {
    return mNy;
}

i32 Grid::cellCount() const {
    return mNx * mNy;
}

f32 Grid::cellSize() const {
    return mCellSize;
}

Vector2D Grid::cellCenter() const {
    return mCellCenter;
}

f32* Grid::data() {
    return mData;
}

f32 Grid::interp(const Vector2D& grid_pos) const {
    // See page 29 for averaging example.
    const Vector2D upper_bound =
        Vector2D(static_cast<f32>(mNx) - cBoundsFactor,
                 static_cast<f32>(mNy) - cBoundsFactor);
    const Vector2D pos =
        (grid_pos - mCellCenter).clamped(Vector2D(0.0f), upper_bound);

    const Vector2i cell00 = Vector2i(pos[0], pos[1]);
    const Vector2i cell01 = cell00 + Vector2i(0, 1);
    const Vector2i cell10 = cell00 + Vector2i(1, 0);
    const Vector2i cell11 = cell00 + Vector2i(1, 1);

    const f32 x0 = static_cast<f32>(cell00[0]);
    const f32 y0 = static_cast<f32>(cell00[1]);

    const f32 x = pos[0] - x0;
    const f32 y = pos[1] - y0;

    const f32 v00 = (*this)(cell00[0], cell00[1]);
    const f32 v01 = (*this)(cell01[0], cell01[1]);
    const f32 v10 = (*this)(cell10[0], cell10[1]);
    const f32 v11 = (*this)(cell11[0], cell11[1]);

    return (1 - x) * (1 - y) * v00 + (1 - x) * y * v01 + x * (1 - y) * v10 +
           x * y * v11;
}

Grid Grid::advect(const Grid& u, const Grid& v, const f32 dt) {
    // Page 32.
    Grid next(mNy, mNx, mCellCenter, mCellSize);

    for (i32 iy = 0; iy < mNy; ++iy) {
        for (i32 ix = 0; ix < mNx; ++ix) {
            // Construct the gridspace position corresponding at the center of
            // the current cell.
            const Vector2D grid_pos = Vector2D(ix, iy) + mCellCenter;

            // Semi-Lagrangian backwards integration in time over (u, v).
            const Vector2D initial_pos = backtrace(grid_pos, u, v, dt);

            // Interpolate from from the grid and set the new value.
            next(ix, iy) = interp(initial_pos);
        }
    }

    return next;
}

Vector2D Grid::toGridSpace(const Vector2D& world_pos) const {
    const Vector2D upper_bound = Vector2D(width() - cBoundsFactor * mCellSize,
                                          height() - cBoundsFactor * mCellSize);
    return (world_pos / mCellSize - mCellCenter)
        .clamped(Vector2D(0.0f), upper_bound);
}

Vector2D Grid::toWorldSpace(const Vector2D& grid_pos) const {
    return (grid_pos + mCellCenter) * mCellSize;
}

void Grid::fill(const f32 value) {
    std::fill_n(mData, mNy * mNx, value);
}

void Grid::add(const Vector2D& world_pos, const Vector2D& size,
               const f32 value) {
    const Vector2D& region_lower = world_pos / mCellSize - mCellCenter;
    const Vector2D& region_upper = (world_pos + size) / mCellSize - mCellCenter;

    const Vector2i low = Vector2i(region_lower[0], region_lower[1]);
    const Vector2i upp = Vector2i(region_upper[0], region_upper[1]);

    for (i32 iy = low[1]; iy < upp[1]; ++iy) {
        for (i32 ix = low[0]; ix < upp[0]; ++ix) {
            if (std::fabs((*this)(ix, iy)) < std::fabs(value))
                (*this)(ix, iy) = value;
        }
    }
}

f32 Grid::max() const {
    return *std::max_element(mData, mData + cellCount());
}

f32 Grid::min() const {
    return *std::min_element(mData, mData + cellCount());
}

Vector2D Grid::backtrace(const Vector2D& grid_pos, const Grid& u, const Grid& v,
                         const f32 dt) const {
    return euler(grid_pos, u, v, dt);
}

Vector2D Grid::euler(const Vector2D& grid_pos, const Grid& u, const Grid& v,
                     const f32 dt) const {
    const Vector2D v1 =
        Vector2D(u.interp(grid_pos), v.interp(grid_pos)) / mCellSize;
    return grid_pos - dt * v1;
}

Vector2D Grid::RK2(const Vector2D& grid_pos, const Grid& u, const Grid& v,
                   const f32 dt) const {
    const Vector2D v1 =
        Vector2D(u.interp(grid_pos), v.interp(grid_pos)) / mCellSize;
    const Vector2D pos_mid = grid_pos - 0.5f * dt * v1;
    const Vector2D v2 =
        Vector2D(u.interp(pos_mid), v.interp(pos_mid)) / mCellSize;
    const Vector2D pos_orig = grid_pos - dt * v2;
    return pos_orig;
}

f32 Grid::width() const {
    return static_cast<f32>(mNx) * mCellSize;
}

f32 Grid::height() const {
    return static_cast<f32>(mNy) * mCellSize;
}
