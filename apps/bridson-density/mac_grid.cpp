#include "mac_grid.hpp"

MACGrid::MACGrid(const i32 rows, const i32 cols, const f32 cell_size)
    : u(rows, cols + 1, Vector2D(0.0f, 0.5f), cell_size),
      v(rows + 1, cols, Vector2D(0.5f, 0.0f), cell_size),
      p(rows, cols, Vector2D(0.5f, 0.5f), cell_size),
      d(rows, cols, Vector2D(0.5f, 0.5f), cell_size),
      label(rows, cols),
      mNx(cols),
      mNy(rows),
      mFacesX(cols + 1),
      mFacesY(rows + 1),
      mCellSize(cell_size) {
    u.fill(0.0f);
    v.fill(0.0f);
    p.fill(0.0f);
    d.fill(0.0f);

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

i32 MACGrid::facesX() const {
    return mFacesX;
}

i32 MACGrid::facesY() const {
    return mFacesY;
}

bool MACGrid::isEmpty(const i32 i, const i32 j) const {
    return label(i, j) == Label::Empty;
}

bool MACGrid::isFluid(const i32 i, const i32 j) const {
    return label(i, j) == Label::Fluid;
}

bool MACGrid::isSolid(const i32 i, const i32 j) const {
    return label(i, j) == Label::Solid;
}

void MACGrid::updateLabels() {
    // A cell has fluid in it if it has non-zero density.
    for (i32 j = 0; j < mNy; ++j) {
        for (i32 i = 0; i < mNx; ++i) {
            if (label(i, j) == Label::Solid) {
                continue;
            }

            if (d(i, j) > 0.0f) {
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
