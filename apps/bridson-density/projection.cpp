#include "projection.hpp"

Projection::Projection(MACGrid& mac) : mMac(mac), mDiv(mac.nx() * mac.ny()) {
}

void Projection::operator()(const f32 dt, const f32 density) {
    // In general, projection subtracts the pressure gradient from the advected
    // velocity field with external forces applied and enforces the velocity
    // field to be divergence-free. Following Bridson, it also enforces
    // solid-wall boundaries.

    buildDivergences();
    solvePressureEquation(dt, density);
    makeDivergenceFree(dt, density);
}

void Projection::buildDivergences() {
    const f32 scale = -1.0f / mMac.cellSize();

    for (i32 iy = 0; iy < mMac.ny(); ++iy) {
        for (i32 ix = 0; ix < mMac.nx(); ++ix) {
            // Equation 5.4
            const Index index = iy * mMac.nx() + ix;
            mDiv[index] = scale * (mMac.u(ix + 1, iy) - mMac.u(ix, iy) +
                                   mMac.v(ix, iy + 1) - mMac.v(ix, iy));
        }
    }
}

void Projection::solvePressureEquation(const f32 dt, const f32 density) {
    // Coeffiecient from equation 5.1
    const f32 scale = dt / (density * mMac.cellSize() * mMac.cellSize());

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
                const f32 p = (mDiv[index] - off_diag) / diag;

                max_delta = std::fmax(max_delta, std::fabs(mMac.p(ix, iy) - p));

                mMac.p(ix, iy) = p;
            }
        }

        if (max_delta < 1e-5) {
            return;
        }
    }
}

void Projection::makeDivergenceFree(const f32 dt, const f32 density) {
    const f32 scale = dt / (density * mMac.cellSize());

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
