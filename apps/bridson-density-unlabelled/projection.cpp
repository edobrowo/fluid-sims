#include "projection.hpp"

#include <algorithm>

Projection::Projection(MACGrid& mac)
    : mMac(mac),
      mDiv(mac.cellCount()),
      mAdiag(mac.cellCount()),
      mAx(mac.cellCount()),
      mAy(mac.cellCount()),
      mPressure(mac.cellCount()),
      mAux(mac.cellCount()),
      mSearch(mac.cellCount()),
      mPreconditioner(mac.cellCount()) {
}

void Projection::operator()(const f64 dt, const f64 density) {
    // In general, projection subtracts the pressure gradient from the advected
    // velocity field with external forces applied and enforces the velocity
    // field to be divergence-free. Following Bridson, it also enforces
    // solid-wall boundaries.

    const f64 tau = 0.97;
    const f64 sigma = 0.25;

    buildDivergences();
    println("mDiv\n{}", mDiv);
    buildPressureMatrix(dt, density);
    println("mAdiag\n{}", mAdiag);
    println("mAx\n{}", mAx);
    println("mAy\n{}", mAy);
    solvePressureEquation(tau, sigma);
    println("mPrecon\n{}", mPreconditioner);
    println("mPressure\n{}", mPressure);

    // Populate pressure grid with pressure solutions.
    for (i32 j = 0; j < mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            const Index index = j * mMac.nx() + i;
            mMac.p(i, j) = mPressure[index];
        }
    }

    applyPressureUpdate(dt, density);
    println("U\n{}", mMac.u);
    println("V\n{}", mMac.v);
}

void Projection::buildDivergences() {
    // Page 72, Figure 5.3.

    const f64 scale = 1.0 / mMac.cellSize();

    for (i32 j = 0; j < mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            const Index index = j * mMac.nx() + i;
            // Equation 5.4
            mDiv[index] = -scale * (mMac.u(i + 1, j) - mMac.u(i, j) +
                                    mMac.v(i, j + 1) - mMac.v(i, j));
        }
    }
}

void Projection::buildPressureMatrix(const f64 dt, const f64 density) {
    // Page 78, Figure 5.5.

    const f64 scale = dt / (density * mMac.cellSize() * mMac.cellSize());

    std::fill(mAdiag.begin(), mAdiag.end(), 0.0);
    std::fill(mAx.begin(), mAx.end(), 0.0);
    std::fill(mAy.begin(), mAy.end(), 0.0);

    for (i32 j = 0; j < mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            const Index index = j * mMac.nx() + i;

            // Enforce solid-wall boundaries at the edges of the viewport.
            if (i < mMac.nx() - 1) {
                mAdiag[index] += scale;
                mAdiag[index + 1] += scale;
                mAx[index] = -scale;
            }

            if (j < mMac.ny() - 1) {
                mAdiag[index] += scale;
                mAdiag[index + mMac.nx()] += scale;
                mAy[index] = -scale;
            }
        }
    }
}

void Projection::solvePressureEquation(const f64 tuning, const f64 safety) {
    buildPreconditioner(tuning, safety);

    // Initial guess of zeros.
    mPressure.fill(0.0);

    // Tolerance for early return.
    const f64 tol = 1e-5;

    applyPreconditioner(mAux, mDiv);
    mSearch = mAux;

    if (mDiv.infinityNorm() < tol)
        return;

    f64 sigma = dot(mAux, mDiv);

    for (i32 iter = 0; iter < cNumberOfCGIterations; ++iter) {
        applyA(mAux, mSearch);

        const f64 alpha = sigma / dot(mAux, mSearch);

        mPressure = mPressure + alpha * mSearch;
        mDiv = mDiv + -alpha * mAux;

        if (mDiv.infinityNorm() < tol)
            return;

        applyPreconditioner(mAux, mDiv);

        const f64 sigma_new = dot(mAux, mDiv);
        const f64 beta = sigma_new / sigma;

        mSearch = mAux + beta * mSearch;
        sigma = sigma_new;
    }
}

void Projection::applyPressureUpdate(const f64 dt, const f64 density) {
    // Based on page 71, Figure 5.2.

    const f64 scale = dt / (density * mMac.cellSize());

    // Application of Equation 5.1.
    for (i32 j = 0; j < mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            const f64 p = scale * mMac.p(i, j);
            mMac.u(i, j) -= p;
            mMac.u(i + 1, j) += p;
            mMac.v(i, j) -= p;
            mMac.v(i, j + 1) += p;
        }
    }

    // Boundary conditions. Treat the boundaries of the window as a solid
    // boundary around the fluid.
    for (i32 j = 0; j < mMac.ny(); ++j) {
        mMac.u(0, j) = 0.0;
        mMac.u(mMac.nx(), j) = 0.0;
    }

    for (i32 i = 0; i < mMac.nx(); ++i) {
        mMac.v(i, 0) = 0.0;
        mMac.v(i, mMac.ny()) = 0.0;
    }
}

void Projection::buildPreconditioner(const f64 tuning, const f64 safety) {
    // Page 87, Figure 5.7.
    // `tuning` is tau, `safety` is sigma.

    for (i32 j = 0; j < mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            const Index index = j * mMac.nx() + i;

            f64 e = mAdiag[index];

            // Enforce solid-wall boundaries at the edges of the viewport.
            if (i > 0) {
                const Index prev = index - 1;
                const f64 x = mAx[prev] * mPreconditioner[prev];
                const f64 y = mAy[prev] * mPreconditioner[prev];
                e = e - (x * x + tuning * x * y);
            }

            if (j > 0) {
                const Index prev = index - mMac.nx();
                const f64 x = mAx[prev] * mPreconditioner[prev];
                const f64 y = mAy[prev] * mPreconditioner[prev];
                e = e - (y * y + tuning * x * y);
            }

            if (e < safety * mAdiag[index]) {
                e = mAdiag[index];
            }

            mPreconditioner[index] = 1.0 / std::sqrt(e);
        }
    }
}

void Projection::applyPreconditioner(VectorXD& dst, const VectorXD& a) {
    // Page 87, Figure 5.8.

    for (i32 j = 0; j < mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            const Index index = j * mMac.nx() + i;

            f64 t = a[index];

            // Enforce solid-wall boundaries at the edges of the viewport.
            if (i > 0) {
                const Index prev = index - 1;
                t -= mAx[prev] * mPreconditioner[prev] * dst[prev];
            }

            if (j > 0) {
                const Index prev = index - mMac.nx();
                t -= mAy[prev] * mPreconditioner[prev] * dst[prev];
            }

            dst[index] = t * mPreconditioner[index];
        }
    }

    for (i32 j = mMac.ny() - 1; j >= 0; --j) {
        for (i32 i = mMac.nx() - 1; i >= 0; --i) {
            const Index index = j * mMac.nx() + i;

            f64 t = dst[index];

            // Enforce solid-wall boundaries at the edges of the viewport.
            if (i < mMac.nx() - 1) {
                t -= mAx[index] * mPreconditioner[index] * dst[index + 1];
            }

            if (j < mMac.ny() - 1) {
                t -= mAy[index] * mPreconditioner[index] *
                     dst[index + mMac.nx()];
            }

            dst[index] = t * mPreconditioner[index];
        }
    }
}

void Projection::applyA(VectorXD& dst, const VectorXD& b) {
    for (i32 j = 0; j < mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            const Index index = j * mMac.nx() + i;

            f64 t = mAdiag[index] * b[index];

            if (i > 0)
                t += mAx[index - 1] * b[index - 1];

            if (j > 0)
                t += mAy[index - mMac.nx()] * b[index - mMac.nx()];

            if (i < mMac.nx() - 1)
                t += mAx[index] * b[index + 1];

            if (j < mMac.ny() - 1)
                t += mAy[index] * b[index + mMac.nx()];

            dst[index] = t;
        }
    }
}
