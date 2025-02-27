#include "mac_grid.hpp"

MACGrid::MACGrid(const Size rows, const Size cols, const f32 cell_size)
    : mU(rows, cols, Vector2D(0.0f, 0.5f), cell_size),
      mV(rows, cols, Vector2D(0.5f, 0.0f), cell_size),
      mP(rows, cols, cell_size),
      mD(rows, cols, cell_size),
      mT(rows, cols, cell_size) {
}

Vector2D MACGrid::velocity(const Vector2D& pos) const {
    return Vector2D(
        //
    );
}
