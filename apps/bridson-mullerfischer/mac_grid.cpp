#include "mac_grid.hpp"

MACGrid::MACGrid(const Size rows, const Size cols, const f32 cell_size)
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

Size MACGrid::cols() const {
    return mCols;
}

Size MACGrid::rows() const {
    return mRows;
}

Size MACGrid::facesX() const {
    return mFacesX;
}

Size MACGrid::facesY() const {
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

// TODO: cell center?
Vector2D MACGrid::cellPosition(const Index x, const Index y) const {
    return Vector2D(x * mCellSize, y * mCellSize);
}

bool MACGrid::isCellInBounds(const Index x, const Index y) const {
    return x < mCols && y < mRows;
}

bool MACGrid::isFaceXInBounds(const Index x, const Index y) const {
    return x < mFacesX;
}

bool MACGrid::isFaceYInBounds(const Index x, const Index y) const {
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

// TODO: clamp (?)
// TODO: use RK3
Vector2D MACGrid::backtrace(const Vector2D& pos, const f32 dt) const {
    const Vector2D& pos_mid = pos - 0.5f * dt * velocity(pos);
    const Vector2D& pos_orig = pos - dt * velocity(pos_mid);
    return pos_orig;
}

f32 MACGrid::cflTimestep() const {
    return mU.cellSize() / std::max(mU.max(), mV.max());
}
