#include "grid.hpp"

#include <algorithm>

#include "math/common.hpp"

Grid::Grid(const Size rows, const Size cols, const Vector2D& cell_center,
           const f32 cell_size)
    : mRows(rows),
      mCols(cols),
      mCellCenter(cell_center.clamped(0.0f, 1.0f)),
      mCellSize(cell_size),
      mData(new f32[rows * cols]) {
}

Grid::Grid(const Size rows, const Size cols, const f32 cell_size)
    : mRows(rows),
      mCols(cols),
      mCellCenter(Vector2D(0.5f, 0.5f)),
      mCellSize(cell_size),
      mData(new f32[rows * cols]) {
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

Size Grid::rows() const {
    return mRows;
}

f32 Grid::height() const {
    return static_cast<float>(mRows) * mCellSize;
}

Size Grid::cols() const {
    return mCols;
}

f32 Grid::width() const {
    return static_cast<float>(mCols) * mCellSize;
}

f32 Grid::cellSize() const {
    return mCellSize;
}

f32* Grid::data() {
    return mData;
}

// TODO: return default instead?
f32 Grid::operator()(const Index row, const Index col) const {
    assertm(row < mRows, "row out of bounds");
    assertm(col < mCols, "col out of bounds");
    return mData[row * mCols + col];
}

// TODO: return default instead?
f32& Grid::operator()(const Index row, const Index col) {
    assertm(row < mRows, "row out of bounds");
    assertm(col < mCols, "col out of bounds");
    return mData[row * mCols + col];
}

f32 Grid::interp(const Vector2D& pos) const {
    const Vector2u cell00 = cell(pos);
    const Vector2u cell01 = cell00 + Vector2u(0, 1);
    const Vector2u cell10 = cell00 + Vector2u(1, 0);
    const Vector2u cell11 = cell00 + Vector2u(1, 1);

    const f32 x0 = mCellSize * static_cast<f32>(cell00[0]);
    const f32 y0 = mCellSize * static_cast<f32>(cell00[1]);

    const f32 t = (pos[0] - x0) / mCellSize;
    const f32 s = (pos[1] - y0) / mCellSize;

    const f32 v00 = (*this)(cell00[1], cell00[0]);
    const f32 v01 = (*this)(cell01[1], cell01[0]);
    const f32 v10 = (*this)(cell10[1], cell10[0]);
    const f32 v11 = (*this)(cell11[1], cell11[0]);

    return (1 - t) * (1 - s) * v00 + (1 - t) * s * v01 + t * (1 - s) * v10 +
           t * s * v11;
}

Vector2u Grid::cell(const Vector2D& pos) const {
    const Vector2D cell_coords = toGridSpace(pos) / mCellSize;
    return Vector2u(static_cast<Index>(cell_coords[0]),
                    static_cast<Index>(cell_coords[1]));
}

Vector2D Grid::toGridSpace(const Vector2D& pos) const {
    return (pos - mCellCenter * mCellSize)
        .clamped(Vector2D(0.0f), Vector2D(width(), height()));
}

void Grid::fill(const f32 value) {
    std::fill_n(mData, mRows * mCols, value);
}
