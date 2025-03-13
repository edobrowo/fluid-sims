#include "mac_grid.hpp"

MACGrid::MACGrid(const i32 rows, const i32 cols, const f32 cell_size)
    : mU(rows, cols + 1, Vector2D(0.0f, 0.5f), cell_size),
      mV(rows + 1, cols, Vector2D(0.5f, 0.0f), cell_size),
      mP(rows, cols, cell_size),
      mD(rows, cols, cell_size),
      mT(rows, cols, cell_size),
      mCols(cols),
      mRows(rows),
      mFacesX(cols + 1),
      mFacesY(rows + 1),
      mCellSize(cell_size) {
}

i32 MACGrid::cols() const {
    return mCols;
}

i32 MACGrid::rows() const {
    return mRows;
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
    return mCols * mCellSize;
}

f32 MACGrid::height() const {
    return mRows * mCellSize;
}

bool MACGrid::isCellInBounds(const i32 x, const i32 y) const {
    return x < mCols && y < mRows;
}

bool MACGrid::isFaceXInBounds(const i32 x, const i32 y) const {
    return x < mFacesX;
}

bool MACGrid::isFaceYInBounds(const i32 x, const i32 y) const {
    return y < mFacesY;
}

Vector2D MACGrid::velocity(const Vector2D& pos) const {
    return Vector2D(mU.interp(pos), mV.interp(pos));
}

f32 MACGrid::pressure(const Vector2D& pos) const {
    return mP.interp(pos);
}

f32 MACGrid::density(const Vector2D& pos) const {
    return mD.interp(pos);
}

f32 MACGrid::temperature(const Vector2D& pos) const {
    return mT.interp(pos);
}

// TODO: RK3
Vector2D MACGrid::backtrace(const Vector2D& pos, const f32 dt) const {
    // Integrate back in time with RK2.
    const Vector2D& pos_mid = pos - 0.5f * dt * velocity(pos);
    const Vector2D& pos_orig = pos - dt * velocity(pos_mid);
    return pos_orig;
}

f32 MACGrid::cflTimestep() const {
    return mU.cellSize() / std::max(mU.max(), mV.max());
}
