#include "grid.hpp"

#include <algorithm>

#include "math/common.hpp"

Grid::Grid(const i32 rows, const i32 cols, const Vector2D& cell_center,
           const f32 cell_size)
    : mRows(rows),
      mCols(cols),
      mCellCenter(cell_center.clamped(0.0f, 1.0f)),
      mCellSize(cell_size) {
    assertm(mRows > 0, "number of rows must be positive");
    assertm(mCols > 0, "number of cols must be positive");
    assertm(mCellSize != 0.0f, "cell size must be nonzero");
    mData = new f32[rows * cols];
}

Grid::Grid(const i32 rows, const i32 cols, const f32 cell_size)
    : mRows(rows),
      mCols(cols),
      mCellCenter(Vector2D(0.5f, 0.5f)),
      mCellSize(cell_size) {
    assertm(mRows > 0, "number of rows must be positive");
    assertm(mCols > 0, "number of cols must be positive");
    assertm(mCellSize != 0.0f, "cell size must be nonzero");
    mData = new f32[rows * cols];
}

Grid::Grid(const Grid& other)
    : mRows(other.mRows),
      mCols(other.mCols),
      mCellCenter(other.mCellCenter),
      mCellSize(other.mCellSize),
      mData(new f32[other.mRows * other.mCols]) {
    std::copy(other.mData, other.mData + (mRows * mCols), mData);
}

Grid& Grid::operator=(const Grid& other) {
    mRows = other.mRows;
    mCols = other.mCols;
    mCellCenter = other.mCellCenter;
    mCellSize = other.mCellSize;
    std::copy(other.mData, other.mData + (mRows * mCols), mData);
    return *this;
}

Grid::~Grid() {
    delete[] mData;
}

i32 Grid::cols() const {
    return mCols;
}

i32 Grid::rows() const {
    return mRows;
}

i32 Grid::size() const {
    return mCols * mRows;
}

f32 Grid::cellSize() const {
    return mCellSize;
}

f32 Grid::width() const {
    return static_cast<float>(mCols) * mCellSize;
}

f32 Grid::height() const {
    return static_cast<float>(mRows) * mCellSize;
}

f32 Grid::operator()(const i32 x, const i32 y) const {
    assertm(x >= 0, "x out of bounds");
    assertm(x < mCols, "x out of bounds");
    assertm(y >= 0, "y out of bounds");
    assertm(y < mRows, "y out of bounds");
    return mData[y * mCols + x];
}

f32& Grid::operator()(const i32 x, const i32 y) {
    assertm(x >= 0, "x out of bounds");
    assertm(x < mCols, "x out of bounds");
    assertm(y >= 0, "y out of bounds");
    assertm(y < mRows, "y out of bounds");
    return mData[y * mCols + x];
}

f32 Grid::interp(const Vector2D& pos) const {
    const Vector2i cell00 = positionToCell(pos);
    const Vector2i cell01 = cell00 + Vector2i(0, 1);
    const Vector2i cell10 = cell00 + Vector2i(1, 0);
    const Vector2i cell11 = cell00 + Vector2i(1, 1);

    const f32 x0 = mCellSize * static_cast<f32>(cell00[0]);
    const f32 y0 = mCellSize * static_cast<f32>(cell00[1]);

    const f32 t = (pos[0] - x0) / mCellSize;
    const f32 s = (pos[1] - y0) / mCellSize;

    const f32 v00 = (*this)(cell00[0], cell00[1]);
    const f32 v01 = (*this)(cell01[0], cell01[1]);
    const f32 v10 = (*this)(cell10[0], cell10[1]);
    const f32 v11 = (*this)(cell11[0], cell11[1]);

    return (1 - t) * (1 - s) * v00 + (1 - t) * s * v01 + t * (1 - s) * v10 +
           t * s * v11;
}

Vector2D Grid::cellToPosition(const Vector2i& cell) const {
    const Vector2D cell_coords(static_cast<float>(cell[0]),
                               static_cast<float>(cell[1]));
    const Vector2D pos = (cell_coords + mCellCenter) * mCellSize;
    return pos;
}

Vector2i Grid::positionToCell(const Vector2D& pos) const {
    const Vector2D adjusted_pos = pos - mCellCenter * mCellSize;

    const Vector2D bounds = Vector2D(width() - cBoundsFactor * mCellSize,
                                     height() - cBoundsFactor * mCellSize);
    const Vector2D clamped_pos = adjusted_pos.clamped(Vector2D(0.0f), bounds);

    const Vector2D cell_coords = clamped_pos / mCellSize;

    return Vector2i(static_cast<i32>(cell_coords[0]),
                    static_cast<i32>(cell_coords[1]));
}

f32 Grid::max() const {
    return *std::max_element(mData, mData + size());
}

f32 Grid::min() const {
    return *std::min_element(mData, mData + size());
}

void Grid::fill(const f32 value) {
    std::fill_n(mData, mRows * mCols, value);
}

void Grid::fill(const Vector2i& lower, const Vector2i& upper, const f32 value) {
    for (i32 y = lower[1]; y < upper[1]; ++y) {
        for (i32 x = lower[0]; x < upper[0]; ++x) {
            (*this)(x, y) = value;
        }
    }
}

f32* Grid::data() {
    return mData;
}
