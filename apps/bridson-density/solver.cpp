#include "solver.hpp"

Solver::Solver(const Config& config)
    : mMac(config.rows, config.cols, config.cellSize),
      mTimestep(config.timestep),
      mDensity(config.density),
      mAdvectDensity(mMac.d, mMac.u, mMac.v),
      mAdvectU(mMac.u, mMac.u, mMac.v),
      mAdvectV(mMac.v, mMac.u, mMac.v) {
}

f32 Solver::color(const Vector2i& cell) const {
    return mMac.d(cell[0], cell[1]);
}

void Solver::step() {
    // See Page 20.
    // Advection occurs after projection since the first advection
    // would do nothing (I think), but also you can add external forces outside
    // of the solver. Forces are added by the client caller (see addDensity and
    // addVelocity).

    // 1. Project the pressure to make the velocity field divergence free.
    project();

    // 2. Advect density and velocity.
    advect();
}

void Solver::project() {
    // In general, projection subtracts the pressure gradient from the advected
    // velocity field with external forces applied and enforces the velocity
    // field to be divergence-free. In Bridson, it also enforces solid-wall
    // boundaries.

    buildDivergences();
    solvePressureEquation();
    makeDivergenceFree();
}

void Solver::advect() {
    mAdvectDensity(mTimestep);
    mAdvectDensity.swap();

    mAdvectU(mTimestep);
    mAdvectV(mTimestep);

    mAdvectU.swap();
    mAdvectV.swap();
}

void Solver::buildDivergences() {
    const f32 scale = -1.0f / mMac.cellSize();

    for (i32 iy = 0; iy < mMac.ny(); ++iy) {
        for (i32 ix = 0; ix < mMac.nx(); ++ix) {
            // Equation 5.4
            const Index index = iy * mMac.nx() + ix;
            mMac.div[index] = scale * (mMac.u(ix + 1, iy) - mMac.u(ix, iy) +
                                       mMac.v(ix, iy + 1) - mMac.v(ix, iy));
        }
    }
}

void Solver::solvePressureEquation() {
    // Coeffiecient from equation 5.1
    const f32 scale =
        mTimestep / (mDensity * mMac.cellSize() * mMac.cellSize());

    // Solve Poisson equation for pressure projection using Gauss-Seidel
    // iteration.
    f32 max_delta = 0.0f;
    for (Index iter = 0; iter < cNumberOfGSIterations; ++iter) {
        max_delta = 0.0f;

        for (i32 iy = 0; iy < mMac.ny(); ++iy) {
            for (i32 ix = 0; ix < mMac.nx(); ++ix) {
                f32 diag = 0.0f;
                f32 off_diag = 0.0f;

                // Assume solid boundary condition beyond the grid.
                if (ix > 0) {
                    diag += scale;
                    off_diag -= scale * mMac.p(ix - 1, iy);
                }
                if (ix < mMac.nx() - 1) {
                    diag += scale;
                    off_diag -= scale * mMac.p(ix + 1, iy);
                }

                if (iy > 0) {
                    diag += scale;
                    off_diag -= scale * mMac.p(ix, iy - 1);
                }
                if (iy < mMac.ny() - 1) {
                    diag += scale;
                    off_diag -= scale * mMac.p(ix, iy + 1);
                }

                const i32 index = iy * mMac.nx() + ix;
                const f32 p = (mMac.div[index] - off_diag) / diag;

                max_delta = std::fmax(max_delta, std::fabs(mMac.p(ix, iy) - p));

                mMac.p(ix, iy) = p;
            }
        }

        if (max_delta < 1e-5) {
            return;
        }
    }
}

void Solver::makeDivergenceFree() {
    const f32 scale = mTimestep / (mDensity * mMac.cellSize());

    for (i32 iy = 0; iy < mMac.ny(); ++iy) {
        for (i32 ix = 0; ix < mMac.nx(); ++ix) {
            const f32 p = scale * mMac.p(ix, iy);
            mMac.u(ix, iy) -= p;
            mMac.u(ix + 1, iy) += p;
            mMac.v(ix, iy) -= p;
            mMac.v(ix, iy + 1) += p;
        }
    }

    // Boundary conditions. Just treat the boundaries of the window as a solid
    // boundary around the fluid.
    for (i32 iy = 0; iy < mMac.ny(); ++iy) {
        mMac.u(0, iy) = 0.0f;
        mMac.u(mMac.nx(), iy) = 0.0f;
    }
    for (i32 ix = 0; ix < mMac.nx(); ++ix) {
        mMac.v(ix, 0) = 0.0f;
        mMac.v(ix, mMac.ny()) = 0.0f;
    }
}

void Solver::addDensity(const Vector2D& pos, const Vector2D& size,
                        const f32 d) {
    mMac.d.add(pos, size, d);
}

void Solver::addVelocity(const Vector2D& pos, const Vector2D& size,
                         const Vector2D& u) {
    mMac.u.add(pos, size, u[0]);
    mMac.v.add(pos, size, u[1]);
}
