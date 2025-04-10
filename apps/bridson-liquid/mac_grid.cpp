#include "mac_grid.hpp"

MACGrid::MACGrid(const i32 rows, const i32 cols, const f32 cell_size)
    : u(rows, cols + 1, Vector2D(0.0, 0.5), cell_size),
      v(rows + 1, cols, Vector2D(0.5, 0.0), cell_size),
      p(rows, cols, Vector2D(0.5, 0.5), cell_size),
      s(rows, cols, Vector2D(0.5, 0.5), cell_size),
      label(rows, cols),
      mNx(cols),
      mNy(rows),
      mCellSize(cell_size) {
    u.fill(0.0);
    v.fill(0.0);
    p.fill(0.0);
    s.fill(0.0);

    label.fill(Label::Empty);
    label.setSolidBorder();
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

void MACGrid::updateLabels() {
    label.reset();

    // A cell has fluid in it if the level set value at that cell is negative.
    for (i32 j = 0; j < mNy; ++j) {
        for (i32 i = 0; i < mNx; ++i) {
            if (label.isSolid(i, j)) {
                continue;
            }

            if (s(i, j) < 0.0) {
                label.set(i, j, Label::Fluid);
            } else {
                label.set(i, j, Label::Empty);
            }
        }
    }
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
