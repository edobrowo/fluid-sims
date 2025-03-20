#include "grid.hpp"

#include <algorithm>

#include "math/common.hpp"

namespace {

f32 cerp_clamped(const f32 t, const f32 a, const f32 b, const f32 c,
                 const f32 d) {
    const f32 min_coeff = std::min(a, std::min(b, std::min(c, d)));
    const f32 max_coeff = std::max(a, std::max(b, std::max(c, d)));
    return math::clamp(math::cerp(t, a, b, c, d), min_coeff, max_coeff);
}

}

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
    const Vector2D& grid_pos0 = toGridSpace(world_pos);
    const Vector2D& grid_pos1 = toGridSpace(world_pos + size);

    const i32 ix0 = std::fmax(0.0f, static_cast<i32>(grid_pos0[0]));
    const i32 iy0 = std::fmax(0.0f, static_cast<i32>(grid_pos0[1]));

    const i32 ix1 = std::fmin(mNx, static_cast<i32>(grid_pos1[0]));
    const i32 iy1 = std::fmin(mNy, static_cast<i32>(grid_pos1[1]));

    const Vector2D g0(ix0, iy0);
    const Vector2D g1(ix1, iy1);

    for (i32 iy = iy0; iy < iy1; ++iy) {
        for (i32 ix = ix0; ix < ix1; ++ix) {
            const Vector2D curr(ix, iy);

            const Vector2D vn =
                ((2.0f * (curr + Vector2D(0.5f)) * mCellSize - (g0 + g1)) /
                 (g1 - g0));
            const f32 l = std::min(std::fabs(vn.length()), 1.0f);

            const f32 v = math::hermite(l) * value;

            if (std::fabs((*this)(ix, iy)) < std::fabs(v))
                (*this)(ix, iy) = v;
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
    return lerp(grid_pos);
}

Vector2D Grid::clampToGrid(const Vector2D& grid_pos) const {
    const Vector2D upper_bound =
        Vector2D(static_cast<f32>(mNx) - cGridClampOffset,
                 static_cast<f32>(mNy) - cGridClampOffset);
    return (grid_pos - mCellCenter).clamped(Vector2D(0.0f), upper_bound);
}

Vector2D Grid::backtrace(const Vector2D& grid_pos, const Grid& u, const Grid& v,
                         const f32 dt) const {
    return RK3(grid_pos, u, v, dt);
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
    const Vector2D pos1 = grid_pos - 0.5f * dt * v1;

    const Vector2D v2 = Vector2D(u.interp(pos1), v.interp(pos1)) / mCellSize;
    const Vector2D pos2 = grid_pos - dt * v2;

    return pos2;
}

Vector2D Grid::RK3(const Vector2D& grid_pos, const Grid& u, const Grid& v,
                   const f32 dt) const {
    const Vector2D v1 =
        Vector2D(u.interp(grid_pos), v.interp(grid_pos)) / mCellSize;
    const Vector2D pos1 = grid_pos - 0.5f * dt * v1;

    const Vector2D v2 = Vector2D(u.interp(pos1), v.interp(pos1)) / mCellSize;
    const Vector2D pos2 = grid_pos - 0.75f * dt * v2;

    const Vector2D v3 = Vector2D(u.interp(pos2), v.interp(pos2)) / mCellSize;
    const Vector2D pos3 = grid_pos - dt * (2.0f / 9.0f * v1 + 3.0f / 9.0f * v2 +
                                           4.0f / 9.0f * v3);

    return pos3;
}

f32 Grid::lerp(const Vector2D& grid_pos) const {
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
    return math::lerp(y, math::lerp(x, v11, v01), math::lerp(x, v10, v00));
}

f32 Grid::cerp(const Vector2D& grid_pos) const {
    const Vector2D pos = clampToGrid(grid_pos);

    const i32 ix = static_cast<i32>(std::floor(pos[0]));
    const i32 iy = static_cast<i32>(std::floor(pos[1]));

    const f32 x = pos[0] - static_cast<f32>(ix);
    const f32 y = pos[1] - static_cast<f32>(iy);

    const i32 x0 = std::max(ix - 1, 0);
    const i32 x1 = ix;
    const i32 x2 = ix + 1;
    const i32 x3 = std::min(ix + 2, mNx - 1);

    const i32 y0 = std::max(iy - 1, 0);
    const i32 y1 = iy;
    const i32 y2 = iy + 1;
    const i32 y3 = std::min(iy + 2, mNy - 1);

    const f32 q0 = cerp_clamped(x, (*this)(x0, y0), (*this)(x1, y0),
                                (*this)(x2, y0), (*this)(x3, y0));
    const f32 q1 = cerp_clamped(x, (*this)(x0, y1), (*this)(x1, y1),
                                (*this)(x2, y1), (*this)(x3, y1));
    const f32 q2 = cerp_clamped(x, (*this)(x0, y2), (*this)(x1, y2),
                                (*this)(x2, y2), (*this)(x3, y2));
    const f32 q3 = cerp_clamped(x, (*this)(x0, y3), (*this)(x1, y3),
                                (*this)(x2, y3), (*this)(x3, y3));

    return cerp_clamped(y, q0, q1, q2, q3);
}

f32 Grid::width() const {
    return static_cast<f32>(mNx) * mCellSize;
}

f32 Grid::height() const {
    return static_cast<f32>(mNy) * mCellSize;
}
