#include "advection.hpp"

Advection::Advection(Grid& q, Grid& u, Grid& v)
    : mQ(q), mU(u), mV(v), mBack(q) {
}

void Advection::operator()(const f32 dt) {
    // Page 32.
    for (i32 j = 0; j < mQ.ny(); ++j) {
        for (i32 i = 0; i < mQ.nx(); ++i) {
            // Construct the gridspace position at the current cell center.
            const Vector2D grid_pos = Vector2D(i, j) + mQ.cellCenter();

            // Semi-Lagrangian backwards integration in time over (u, v).
            const Vector2D initial_pos = backtrace(grid_pos, dt);

            // Interpolate from from the grid and set the new value.
            mBack(i, j) = mQ.interp(initial_pos);
        }
    }
}

void Advection::swap() {
    std::swap(mQ, mBack);
}

Vector2D Advection::backtrace(const Vector2D& grid_pos, const f32 dt) const {
    return RK3(grid_pos, dt);
}

Vector2D Advection::euler(const Vector2D& grid_pos, const f32 dt) const {
    const Vector2D v1 =
        Vector2D(mU.interp(grid_pos), mV.interp(grid_pos)) / mQ.cellSize();
    return grid_pos - dt * v1;
}

Vector2D Advection::RK2(const Vector2D& grid_pos, const f32 dt) const {
    const Vector2D v1 =
        Vector2D(mU.interp(grid_pos), mV.interp(grid_pos)) / mQ.cellSize();
    const Vector2D pos1 = grid_pos - 0.5f * dt * v1;

    const Vector2D v2 =
        Vector2D(mU.interp(pos1), mV.interp(pos1)) / mQ.cellSize();
    const Vector2D pos2 = grid_pos - dt * v2;

    return pos2;
}

Vector2D Advection::RK3(const Vector2D& grid_pos, const f32 dt) const {
    const Vector2D v1 =
        Vector2D(mU.interp(grid_pos), mV.interp(grid_pos)) / mQ.cellSize();
    const Vector2D pos1 = grid_pos - 0.5f * dt * v1;

    const Vector2D v2 =
        Vector2D(mU.interp(pos1), mV.interp(pos1)) / mQ.cellSize();
    const Vector2D pos2 = grid_pos - 0.75f * dt * v2;

    const Vector2D v3 =
        Vector2D(mU.interp(pos2), mV.interp(pos2)) / mQ.cellSize();
    const Vector2D pos3 = grid_pos - dt * (2.0f / 9.0f * v1 + 3.0f / 9.0f * v2 +
                                           4.0f / 9.0f * v3);

    return pos3;
}
