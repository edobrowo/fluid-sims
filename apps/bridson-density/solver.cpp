#include "solver.hpp"

Solver::Solver(const Config& config)
    : mMac(config.rows, config.cols, config.cellSize),
      mTimestep(config.timestep),
      mDensity(config.density) {
}

f32 Solver::color(const Vector2i& cell) const {
    return mMac.mDensity(cell[0], cell[1]);
}

void Solver::step() {
    // Page 20. Advection occurs after projection since the first advection
    // would do nothing (I think). Forces are added by the client caller (see
    // addDensity and addVelocity).

    buildDivergences();
    project();
    makeDivergenceFree();

    const Grid density = mMac.mDensity.advect(mMac.mU, mMac.mV, mTimestep);
    const Grid u = mMac.mU.advect(mMac.mU, mMac.mV, mTimestep);
    const Grid v = mMac.mV.advect(mMac.mU, mMac.mV, mTimestep);

    mMac.mDensity = density;
    mMac.mU = u;
    mMac.mV = v;
}

void Solver::project() {
    constexpr Size MAX_GS_ITER = 600;

    // Coeffiecient from 5.1/5.2
    const f32 scale =
        mTimestep / (mDensity * mMac.cellSize() * mMac.cellSize());

    f32 max_delta = 0.0f;
    for (Index iter = 0; iter < MAX_GS_ITER; ++iter) {
        max_delta = 0.0f;
        for (i32 iy = 0; iy < mMac.ny(); ++iy) {
            for (i32 ix = 0; ix < mMac.nx(); ++ix) {
                f32 diag = 0.0f;
                f32 off_diag = 0.0f;

                // Assume solid boundary condition beyond the grid.
                if (ix > 0) {
                    diag += scale;
                    off_diag -= scale * mMac.mPressure(ix - 1, iy);
                }
                if (ix < mMac.nx() - 1) {
                    diag += scale;
                    off_diag -= scale * mMac.mPressure(ix + 1, iy);
                }

                if (iy > 0) {
                    diag += scale;
                    off_diag -= scale * mMac.mPressure(ix, iy - 1);
                }
                if (iy < mMac.ny() - 1) {
                    diag += scale;
                    off_diag -= scale * mMac.mPressure(ix, iy + 1);
                }

                const i32 index = iy * mMac.nx() + ix;
                const f32 p = (mMac.mDiv[index] - off_diag) / diag;

                max_delta =
                    std::fmax(max_delta, std::fabs(mMac.mPressure(ix, iy) - p));

                mMac.mPressure(ix, iy) = p;
            }
        }

        if (max_delta < 1e-5) {
            return;
        }
    }
}

void Solver::buildDivergences() {
    const f32 scale = -1.0f / mMac.cellSize();

    for (i32 iy = 0; iy < mMac.ny(); ++iy) {
        for (i32 ix = 0; ix < mMac.nx(); ++ix) {
            // Compute divergence at the given cell (using 2D finite
            // differences).
            const Index idx = iy * mMac.nx() + ix;
            mMac.mDiv[idx] = scale * (mMac.mU(ix + 1, iy) - mMac.mU(ix, iy) +
                                      mMac.mV(ix, iy + 1) - mMac.mV(ix, iy));
        }
    }
}

void Solver::makeDivergenceFree() {
    const f32 scale = mTimestep / (mDensity * mMac.cellSize());

    for (i32 iy = 0; iy < mMac.ny(); ++iy) {
        for (i32 ix = 0; ix < mMac.nx(); ++ix) {
            const f32 p = scale * mMac.mPressure(ix, iy);
            mMac.mU(ix, iy) -= p;
            mMac.mU(ix + 1, iy) += p;
            mMac.mV(ix, iy) -= p;
            mMac.mV(ix, iy + 1) += p;
        }
    }

    for (i32 iy = 0; iy < mMac.ny(); ++iy) {
        mMac.mU(0, iy) = 0.0f;
        mMac.mU(mMac.nx(), iy) = 0.0f;
    }

    for (i32 ix = 0; ix < mMac.nx(); ++ix) {
        mMac.mV(ix, 0) = 0.0f;
        mMac.mV(ix, mMac.ny()) = 0.0f;
    }
}

void Solver::addDensity(const Vector2D& pos, const Vector2D& size,
                        const f32 d) {
    mMac.mDensity.add(pos, size, d);
}

void Solver::addVelocity(const Vector2D& pos, const Vector2D& size,
                         const Vector2D& u) {
    mMac.mU.add(pos, size, u[0]);
    mMac.mV.add(pos, size, u[1]);
}
