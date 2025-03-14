#include "mac_grid.hpp"

MACGrid::MACGrid(const i32 rows, const i32 cols, const f32 cell_size)
    : mU(rows, cols + 1, Vector2D(0.0f, 0.5f), cell_size),
      mV(rows + 1, cols, Vector2D(0.5f, 0.0f), cell_size),
      mPressure(rows, cols, cell_size),
      mDiv(rows * cols),
      mDensity(rows, cols, cell_size),
      mTemperature(rows, cols, cell_size),
      mNx(cols),
      mNy(rows),
      mFacesX(cols + 1),
      mFacesY(rows + 1),
      mCellSize(cell_size) {
    mU.fill(0.0f);
    mV.fill(0.0f);
    mPressure.fill(0.0f);
    mDensity.fill(0.0f);
    mTemperature.fill(0.0f);
}

i32 MACGrid::nx() const {
    return mNx;
}

i32 MACGrid::ny() const {
    return mNy;
}

i32 MACGrid::facesX() const {
    return mFacesX;
}

i32 MACGrid::facesY() const {
    return mFacesY;
}

f32 MACGrid::cellSize() const {
    return mCellSize;
}

f32 MACGrid::width() const {
    return mNx * mCellSize;
}

f32 MACGrid::height() const {
    return mNy * mCellSize;
}

bool MACGrid::isCellInBounds(const i32 ix, const i32 iy) const {
    return ix < mNx && iy < mNy;
}

bool MACGrid::isFaceXInBounds(const i32 ix, const i32 iy) const {
    return ix < mFacesX;
}

bool MACGrid::isFaceYInBounds(const i32 ix, const i32 iy) const {
    return iy < mFacesY;
}

f32 MACGrid::cflTimestep() const {
    return mU.cellSize() / std::max(mU.max(), mV.max());
}
