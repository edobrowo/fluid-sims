#include "grid.hpp"

#include <algorithm>

#include "math/numeric.hpp"

namespace {

f64 cerp_clamped(
    const f64 t, const f64 a, const f64 b, const f64 c, const f64 d) {
    const f64 min_coeff = std::min(a, std::min(b, std::min(c, d)));
    const f64 max_coeff = std::max(a, std::max(b, std::max(c, d)));
    return math::clamp(math::cerp(t, a, b, c, d), min_coeff, max_coeff);
}

}

Grid::Grid(const i32 rows,
           const i32 cols,
           const Vector2D& cell_center,
           const f64 cell_size)
    : mNx(cols),
      mNy(rows),
      mCellCenter(cell_center.clamped(0.0, 1.0)),
      mCellSize(cell_size) {
    assertm(mNy > 0, "number of rows must be positive");
    assertm(mNx > 0, "number of cols must be positive");
    assertm(mCellSize > 0.0, "cell size must be positive");

    mData = new f64[mNx * mNy];
}

Grid::Grid(const Grid& other)
    : mNx(other.mNx),
      mNy(other.mNy),
      mCellCenter(other.mCellCenter),
      mCellSize(other.mCellSize),
      mData(new f64[other.mNx * other.mNy]) {
    std::copy(other.mData, other.mData + (mNx * mNy), mData);
}

Grid& Grid::operator=(const Grid& other) {
    mNx = other.mNx;
    mNy = other.mNy;
    mCellCenter = other.mCellCenter;
    mCellSize = other.mCellSize;

    delete[] mData;
    mData = new f64[mNx * mNy];
    std::copy(other.mData, other.mData + (mNx * mNy), mData);

    return *this;
}

Grid::~Grid() {
    delete[] mData;
}

f64 Grid::operator()(const i32 i, const i32 j) const {
    assertm(i >= 0, "i out of bounds");
    assertm(i < mNx, "i out of bounds");
    assertm(j >= 0, "j out of bounds");
    assertm(j < mNy, "j out of bounds");

    return mData[j * mNx + i];
}

f64& Grid::operator()(const i32 i, const i32 j) {
    assertm(i >= 0, "i out of bounds");
    assertm(i < mNx, "i out of bounds");
    assertm(j >= 0, "j out of bounds");
    assertm(j < mNy, "j out of bounds");

    return mData[j * mNx + i];
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

f64 Grid::cellSize() const {
    return mCellSize;
}

Vector2D Grid::cellCenter() const {
    return mCellCenter;
}

f64* Grid::data() {
    return mData;
}

Vector2D Grid::toGridSpace(const Vector2D& world_pos) const {
    return world_pos / mCellSize - mCellCenter;
}

Vector2D Grid::toWorldSpace(const Vector2D& grid_pos) const {
    return (grid_pos + mCellCenter) * mCellSize;
}

f64 Grid::interp(const Vector2D& grid_pos) const {
    return cerp(grid_pos);
}

Vector2D Grid::grad(const Vector2D& grid_pos) const {
    const i32 i = static_cast<i32>(std::floor(grid_pos[0]));
    const i32 j = static_cast<i32>(std::floor(grid_pos[1]));

    Vector2D g(0.0);

    if (i == 0)
        g[0] = (*this)(i, j) - (*this)(i + 1, j);
    else if (i == nx() - 1)
        g[0] = (*this)(i - 1, j) - (*this)(i, j);
    else if (math::abs((*this)(i - 1, j)) < math::abs((*this)(i + 1, j)))
        g[0] = (*this)(i, j) - (*this)(i + 1, j);
    else
        g[0] = (*this)(i - 1, j) - (*this)(i, j);

    if (j == 0)
        g[1] = (*this)(i, j) - (*this)(i, j + 1);
    else if (j == ny() - 1)
        g[1] = (*this)(i, j - 1) - (*this)(i, j);
    else if (math::abs((*this)(i, j - 1)) < math::abs((*this)(i, j + 1)))
        g[1] = (*this)(i, j) - (*this)(i, j + 1);
    else
        g[1] = (*this)(i, j - 1) - (*this)(i, j);

    return g;
}

void Grid::fill(const f64 value) {
    std::fill_n(mData, mNy * mNx, value);
}

void Grid::add(const Vector2D& world_pos,
               const Vector2D& size,
               const f64 value) {
    const Vector2D& grid_pos0 = toGridSpace(world_pos);
    const Vector2D& grid_pos1 = toGridSpace(world_pos + size);

    const i32 i0 = std::max(0, static_cast<i32>(grid_pos0[0]));
    const i32 j0 = std::max(0, static_cast<i32>(grid_pos0[1]));

    const i32 i1 = std::min(mNx, static_cast<i32>(grid_pos1[0]));
    const i32 j1 = std::min(mNy, static_cast<i32>(grid_pos1[1]));

    const Vector2D g0(i0, j0);
    const Vector2D g1(i1, j1);

    for (i32 j = j0; j <= j1; ++j) {
        for (i32 i = i0; i <= i1; ++i) {
            const Vector2D curr(i, j);

            const Vector2D vn =
                ((2.0 * (curr + Vector2D(0.5)) * mCellSize - (g0 + g1)) /
                 (g1 - g0));
            const f64 l = std::min(std::fabs(vn.length()), 1.0);

            const f64 v = math::hermite(l) * value;

            if (std::fabs((*this)(i, j)) < std::fabs(v))
                (*this)(i, j) = v;
        }
    }
}

f64 Grid::max() const {
    return *std::max_element(mData, mData + cellCount());
}

f64 Grid::min() const {
    return *std::min_element(mData, mData + cellCount());
}

Vector2D Grid::clampToGrid(const Vector2D& grid_pos) const {
    const Vector2D upper_bound =
        Vector2D(static_cast<f64>(mNx) - cGridClampOffset,
                 static_cast<f64>(mNy) - cGridClampOffset);
    return (grid_pos - mCellCenter).clamped(Vector2D(0.0), upper_bound);
}

f64 Grid::lerp(const Vector2D& grid_pos) const {
    const Vector2D pos = clampToGrid(grid_pos);

    const i32 i = static_cast<i32>(std::floor(pos[0]));
    const i32 j = static_cast<i32>(std::floor(pos[1]));

    const f64 x = pos[0] - static_cast<f64>(i);
    const f64 y = pos[1] - static_cast<f64>(j);

    const f64 v00 = (*this)(i, j);
    const f64 v01 = (*this)(i, j + 1);
    const f64 v10 = (*this)(i + 1, j);
    const f64 v11 = (*this)(i + 1, j + 1);

    // See page 29 for averaging example.
    return math::lerp(y, math::lerp(x, v11, v01), math::lerp(x, v10, v00));
}

f64 Grid::cerp(const Vector2D& grid_pos) const {
    const Vector2D pos = clampToGrid(grid_pos);

    const i32 i = static_cast<i32>(std::floor(pos[0]));
    const i32 j = static_cast<i32>(std::floor(pos[1]));

    const f64 x = pos[0] - static_cast<f64>(i);
    const f64 y = pos[1] - static_cast<f64>(j);

    const i32 x0 = std::max(i - 1, 0);
    const i32 x1 = i;
    const i32 x2 = i + 1;
    const i32 x3 = std::min(i + 2, mNx - 1);

    const i32 y0 = std::max(j - 1, 0);
    const i32 y1 = j;
    const i32 y2 = j + 1;
    const i32 y3 = std::min(j + 2, mNy - 1);

    const f64 q0 = cerp_clamped(
        x, (*this)(x0, y0), (*this)(x1, y0), (*this)(x2, y0), (*this)(x3, y0));
    const f64 q1 = cerp_clamped(
        x, (*this)(x0, y1), (*this)(x1, y1), (*this)(x2, y1), (*this)(x3, y1));
    const f64 q2 = cerp_clamped(
        x, (*this)(x0, y2), (*this)(x1, y2), (*this)(x2, y2), (*this)(x3, y2));
    const f64 q3 = cerp_clamped(
        x, (*this)(x0, y3), (*this)(x1, y3), (*this)(x2, y3), (*this)(x3, y3));

    return cerp_clamped(y, q0, q1, q2, q3);
}

f64 Grid::width() const {
    return static_cast<f64>(mNx) * mCellSize;
}

f64 Grid::height() const {
    return static_cast<f64>(mNy) * mCellSize;
}
