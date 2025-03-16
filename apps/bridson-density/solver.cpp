#include "solver.hpp"

Solver::Solver(const Config& config)
    : mMac(config.rows, config.cols, config.cellSize),
      mTimestep(config.timestep),
      mDensity(config.density) {
}

f32 Solver::color(const Vector2i& cell) const {
    return mMac.d(cell[0], cell[1]);
}

void Solver::step() {
    // Page 20. Advection occurs after projection since the first advection
    // would do nothing (I think). Forces are added by the client caller (see
    // addDensity and addVelocity).

    project();

    const Grid density = mMac.d.advect(mMac.u, mMac.v, mTimestep);
    const Grid u = mMac.u.advect(mMac.u, mMac.v, mTimestep);
    const Grid v = mMac.v.advect(mMac.u, mMac.v, mTimestep);

    mMac.d = std::move(density);
    mMac.u = std::move(u);
    mMac.v = std::move(v);
}

void Solver::project() {
    buildDivergences();
    solvePressureEquation();
    makeDivergenceFree();
}

void Solver::buildDivergences() {
    const f32 scale = -1.0f / mMac.cellSize();

    for (i32 iy = 0; iy < mMac.ny(); ++iy) {
        for (i32 ix = 0; ix < mMac.nx(); ++ix) {
            // Compute divergence at the given cell (using 2D finite
            // differences).
            const Index idx = iy * mMac.nx() + ix;
            mMac.div[idx] = scale * (mMac.u(ix + 1, iy) - mMac.u(ix, iy) +
                                     mMac.v(ix, iy + 1) - mMac.v(ix, iy));
        }
    }
}

void Solver::solvePressureEquation() {
    // Coeffiecient from 5.1/5.2
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
