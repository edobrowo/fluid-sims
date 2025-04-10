#include "solver.hpp"

Solver::Solver(const Config& config)
    : mMac(config.rows, config.cols, config.cellSize),
      mTimestep(config.timestep),
      mExtrapolateU(mMac.u, mMac.label),
      mExtrapolateV(mMac.v, mMac.label),
      mAdvectSurface(mMac.s, mMac.u, mMac.v, mMac.label),
      mRedistanceSurface(mMac.s, mMac.label),
      mAdvectU(mMac.u, mMac.u, mMac.v, mMac.label),
      mAdvectV(mMac.v, mMac.u, mMac.v, mMac.label),
      mProject(mMac) {
    const f64 r = 3.0;
    for (i32 j = 0; j < mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            const f64 d =
                std::sqrt(math::sqr(static_cast<f64>(i - mMac.nx() / 2)) +
                          math::sqr(static_cast<f64>(j - mMac.ny() / 2)));
            mMac.s(i, j) = d - r;
        }
    }
}

void Solver::step() {
    // See Page 20.

    // 1. Advect surface level set and velocity.
    mMac.updateLabels();

    mExtrapolateU(mMac.label);
    mExtrapolateV(mMac.label);

    advect();

    // for (i32 j = 0; j < mMac.ny(); ++j) {
    //     for (i32 i = 0; i < mMac.nx(); ++i) {
    //         print("{}, ", mMac.s.grad(Vector2D(i, j) + mMac.s.cellCenter()));
    //     }
    //     println("");
    // }

    // mRedistanceSurface();

    // 2. Add external forces.

    addForces();

    // mMac.updateLabels();

    // 3. Project the pressure to make the velocity field divergence free.

    // project();
}

const Grid& Solver::surface() const {
    return mMac.s;
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
    mAdvectSurface(mTimestep);
    mAdvectSurface.swap();

    mAdvectU(mTimestep);
    mAdvectV(mTimestep);

    mAdvectU.swap();
    mAdvectV.swap();
}

void Solver::addForces() {
    // const Vector2D pos(0.45, 0.2);
    // const Vector2D size(0.1, 0.01);
    // const float d = 1.0;
    // const Vector2D u(0.0, 3.0);

    // mMac.s.add(pos, size, d);
    // mMac.u.add(pos, size, u[0]);
    // mMac.v.add(pos, size, u[1]);

    const f64 g = -0.98;

    for (i32 j = 0; j <= mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            mMac.v(i, j) += g;
        }
    }
}

void Solver::project() {
    mProject(mTimestep);
}
