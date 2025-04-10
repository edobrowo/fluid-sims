#include "solver.hpp"

Solver::Solver(const Config& config)
    : mMac(config.rows, config.cols, config.cellSize),
      mTimestep(config.timestep),
      mDensity(config.density),
      mExtrapolateU(mMac.u, mMac.label),
      mExtrapolateV(mMac.v, mMac.label),
      mAdvectDensity(mMac.d, mMac.u, mMac.v, mMac.label),
      mAdvectU(mMac.u, mMac.u, mMac.v, mMac.label),
      mAdvectV(mMac.v, mMac.u, mMac.v, mMac.label),
      mProject(mMac) {
}

void Solver::step() {
    // See Page 20.

    // 1. Advect density and velocity
    mMac.updateLabels();

    mExtrapolateU(mMac.label);
    mExtrapolateV(mMac.label);

    advect();

    // 2. Add external forces.

    addForces();

    mMac.updateLabels();

    // 3. Project the pressure to make the velocity field divergence free.

    project();
}

const Grid& Solver::density() const {
    return mMac.d;
}

const Grid& Solver::pressure() const {
    return mMac.p;
}

const Grid& Solver::u() const {
    return mMac.u;
}

const Grid& Solver::v() const {
    return mMac.v;
}

const LabelGrid& Solver::label() const {
    return mMac.label;
}

void Solver::advect() {
    mAdvectDensity(mTimestep);
    mAdvectDensity.swap();

    mAdvectU(mTimestep);
    mAdvectV(mTimestep);

    mAdvectU.swap();
    mAdvectV.swap();
}

void Solver::addForces() {
    const Vector2D pos(0.45, 0.2);
    const Vector2D size(0.1, 0.01);
    const float d = 1.0;
    const Vector2D u(0.0, 3.0);

    mMac.d.add(pos, size, d);
    mMac.u.add(pos, size, u[0]);
    mMac.v.add(pos, size, u[1]);
}

void Solver::project() {
    mProject(mTimestep, mDensity);
}
