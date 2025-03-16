#include "grid.hpp"

#include <algorithm>

#include "math/common.hpp"

Grid::Grid(const i32 rows, const i32 cols, const Vector2D& cell_center,
           const f32 cell_size)
    : mNx(cols),
      mNy(rows),
      mCellCenter(cell_center.clamped(0.0f, 1.0f)),
      mCellSize(cell_size) {
    assertm(mNy > 0, "number of rows must be positive");
    assertm(mNx > 0, "number of cols must be positive");
    assertm(mCellSize > 0.0f, "cell size must be positive");

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

Vector2D Grid::toGridSpace(const Vector2D& world_pos) const {
    return world_pos / mCellSize - mCellCenter;
}

Vector2D Grid::toWorldSpace(const Vector2D& grid_pos) const {
    return (grid_pos + mCellCenter) * mCellSize;
}

Grid Grid::advect(const Grid& u, const Grid& v, const f32 dt) {
    Grid next(mNy, mNx, mCellCenter, mCellSize);

    // Page 32.
    for (i32 iy = 0; iy < mNy; ++iy) {
        for (i32 ix = 0; ix < mNx; ++ix) {
            // Construct the gridspace position at the current cell center.
            const Vector2D grid_pos = Vector2D(ix, iy) + mCellCenter;

            // Semi-Lagrangian backwards integration in time over (u, v).
            const Vector2D initial_pos = backtrace(grid_pos, u, v, dt);

            // Interpolate from from the grid and set the new value.
            next(ix, iy) = interp(initial_pos);
        }
    }

    return next;
}

void Grid::fill(const f32 value) {
    std::fill_n(mData, mNy * mNx, value);
}

void Grid::add(const Vector2D& world_pos, const Vector2D& size,
               const f32 value) {
    const Vector2D& start = toGridSpace(world_pos);
    const Vector2D& end = toGridSpace(world_pos + size);

    const i32 iy_start = std::fmax(0.0f, static_cast<i32>(start[1]));
    const i32 iy_end = std::fmin(mNy, static_cast<i32>(end[1]));
    const i32 ix_start = std::fmax(0.0f, static_cast<i32>(start[0]));
    const i32 ix_end = std::fmin(mNx, static_cast<i32>(end[0]));

    for (i32 iy = iy_start; iy < iy_end; ++iy) {
        for (i32 ix = ix_start; ix < ix_end; ++ix) {
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

f32 Grid::interp(const Vector2D& grid_pos) const {
    const Vector2D pos = clampToGrid(grid_pos);

    const i32 ix = static_cast<i32>(std::floor(pos[0]));
    const i32 iy = static_cast<i32>(std::floor(pos[1]));

    const f32 x = pos[0] - static_cast<f32>(ix);
    const f32 y = pos[1] - static_cast<f32>(iy);

    const f32 v00 = (*this)(ix, iy);
    const f32 v01 = (*this)(ix, iy + 1);
    const f32 v10 = (*this)(ix + 1, iy);
    const f32 v11 = (*this)(ix + 1, iy + 1);

    // See page 29 for averaging example.
    return (1 - x) * (1 - y) * v00 + (1 - x) * y * v01 + x * (1 - y) * v10 +
           x * y * v11;
}

Vector2D Grid::clampToGrid(const Vector2D& grid_pos) const {
    const Vector2D upper_bound =
        Vector2D(static_cast<f32>(mNx) - cGridClampOffset,
                 static_cast<f32>(mNy) - cGridClampOffset);
    return (grid_pos - mCellCenter).clamped(Vector2D(0.0f), upper_bound);
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
