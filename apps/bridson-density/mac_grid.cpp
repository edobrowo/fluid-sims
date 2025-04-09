#include "mac_grid.hpp"

MACGrid::MACGrid(const i32 rows, const i32 cols, const f64 cell_size)
    : u(rows, cols + 1, Vector2D(0.0, 0.5), cell_size),
      v(rows + 1, cols, Vector2D(0.5, 0.0), cell_size),
      p(rows, cols, Vector2D(0.5, 0.5), cell_size),
      d(rows, cols, Vector2D(0.5, 0.5), cell_size),
      mNx(cols),
      mNy(rows),
      mCellSize(cell_size) {
    u.fill(0.0);
    v.fill(0.0);
    p.fill(0.0);
    d.fill(0.0);
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

f64 MACGrid::cellSize() const {
    return mCellSize;
}

f64 MACGrid::width() const {
    return mNx * mCellSize;
}

f64 MACGrid::height() const {
    return mNy * mCellSize;
}

f64 MACGrid::cflTimestep() const {
    return u.cellSize() / std::max(u.max(), v.max());
}
