#include "mac_grid.hpp"

MACGrid::MACGrid(const i32 rows, const i32 cols, const f32 cell_size)
    : u(rows, cols + 1, Vector2D(0.0f, 0.5f), cell_size),
      v(rows + 1, cols, Vector2D(0.5f, 0.0f), cell_size),
      p(rows, cols, Vector2D(0.5f, 0.5f), cell_size),
      div(rows * cols),
      d(rows, cols, Vector2D(0.5f, 0.5f), cell_size),
      mNx(cols),
      mNy(rows),
      mFacesX(cols + 1),
      mFacesY(rows + 1),
      mCellSize(cell_size) {
    u.fill(0.0f);
    v.fill(0.0f);
    p.fill(0.0f);
    d.fill(0.0f);
}

i32 MACGrid::nx() const {
    return mNx;
}

i32 MACGrid::ny() const {
    return mNy;
}

i32 MACGrid::cellCount() const {
    return mNx * mNy;
}

f32 MACGrid::cellSize() const {
    return mCellSize;
}

i32 MACGrid::facesX() const {
    return mFacesX;
}

i32 MACGrid::facesY() const {
    return mFacesY;
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

f32 MACGrid::width() const {
    return mNx * mCellSize;
}

f32 MACGrid::height() const {
    return mNy * mCellSize;
}

f32 MACGrid::cflTimestep() const {
    return u.cellSize() / std::max(u.max(), v.max());
}
