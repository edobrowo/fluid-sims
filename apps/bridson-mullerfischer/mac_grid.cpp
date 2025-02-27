#include "mac_grid.hpp"

MACGrid::MACGrid(const Size rows, const Size cols, const f32 cell_size)
    : mU(rows, cols, Vector2D(0.0f, 0.5f), cell_size),
      mV(rows, cols, Vector2D(0.5f, 0.0f), cell_size),
      mP(rows, cols, cell_size),
      mD(rows, cols, cell_size),
      mT(rows, cols, cell_size) {
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
Vector2D MACGrid::backtrace(const Vector2D& pos, const f32 dt) const {
    // Use RK2.
    const Vector2D& v1 = velocity(pos);
    const Vector2D& v2 = velocity(
        Vector2D(pos[0] + 0.5f * dt * v1[0], pos[1] + 0.5f * dt * v1[1]));
    return pos + dt * v2;
}
