#include "solver.hpp"

Solver::Solver(const Config& config)
    : mMac(config.rows, config.cols, config.cellSize),
      mTimestep(config.timestep),
      mDensity(config.density),
      mAdvectDensity(mMac.d, mMac.u, mMac.v),
      mAdvectU(mMac.u, mMac.u, mMac.v),
      mAdvectV(mMac.v, mMac.u, mMac.v),
      mProject(mMac) {
}

f32 Solver::color(const Vector2i& cell) const {
    return mMac.d(cell[0], cell[1]);
}

void Solver::step() {
    // See Page 20.

    // 1. Advect density and velocity.
    advect();

    mMac.updateLabels();

    // 2. Add external forces.
    const Vector2D pos(0.45f, 0.2f);
    const Vector2D size(0.1f, 0.01f);
    const float d = 1.0f;
    const Vector2D u(0.0, 3.0);
    addForces(pos, size, d, u);

    mMac.updateLabels();

    // 3. Project the pressure to make the velocity field divergence free.
    project();

    mMac.updateLabels();
}

void Solver::project() {
    mProject(mTimestep, mDensity);
}

void Solver::advect() {
    mAdvectDensity(mTimestep);
    mAdvectDensity.swap();

    mAdvectU(mTimestep);
    mAdvectV(mTimestep);

    mAdvectU.swap();
    mAdvectV.swap();
}

void Solver::addForces(const Vector2D& pos,
                       const Vector2D& size,
                       const f32 d,
                       const Vector2D& u) {
    mMac.d.add(pos, size, d);
    mMac.u.add(pos, size, u[0]);
    mMac.v.add(pos, size, u[1]);
}
