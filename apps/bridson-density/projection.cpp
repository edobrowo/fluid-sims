#include "projection.hpp"

#include <algorithm>

Projection::Projection(MACGrid& mac)
    : mMac(mac),
      mDiv(mac.cellCount()),
      mAdiag(mac.cellCount()),
      mAx(mac.cellCount()),
      mAy(mac.cellCount()),
      mAux(mac.cellCount()),
      mSearch(mac.cellCount()),
      mPreconditioner(mac.cellCount()) {
}

void Projection::operator()(const f32 dt, const f32 density) {
    // In general, projection subtracts the pressure gradient from the advected
    // velocity field with external forces applied and enforces the velocity
    // field to be divergence-free. Following Bridson, it also enforces
    // solid-wall boundaries.

    const f32 tau = 0.97f;
    const f32 sigma = 0.25;

    buildDivergences();
    buildPressureMatrix(dt, density);
    buildPreconditioner(tau, sigma);
    solvePressureEquation();
    applyPressureUpdate(dt, density);
}

void Projection::buildDivergences() {
    // Page 72, Figure 5.3.

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

void Projection::solvePressureEquation() {
    // Initial guess of zeros.
    mMac.p.fill(0.0f);

    applyPreconditioner(mAux, mDiv);
    mSearch = mAux;

    if (infinityNorm(mDiv) < 1e-5)
        return;

    f32 sigma = dot(mAux, mDiv);

    for (i32 iter = 0; iter < cNumberOfCGIterations; ++iter) {
        matmul(mAux, mSearch);

        const f32 alpha = sigma / dot(mAux, mSearch);

        // Scaled add for grid p.
        for (i32 iy = 0; iy < mMac.ny(); ++iy) {
            for (i32 ix = 0; ix < mMac.nx(); ++ix) {
                const Index index = iy * mMac.nx() + ix;
                mMac.p(ix, iy) = mMac.p(ix, iy) + alpha * mSearch[index];
            }
        }
        scaledAdd(mDiv, mDiv, mAux, -alpha);

        if (infinityNorm(mDiv) < 1e-5)
            return;

        applyPreconditioner(mAux, mDiv);

        const f32 sigma_new = dot(mAux, mDiv);
        scaledAdd(mSearch, mAux, mSearch, sigma_new / sigma);
        sigma = sigma_new;
    }
}

void Projection::applyPressureUpdate(const f32 dt, const f32 density) {
    // Based on page 71, Figure 5.2.

    const f32 scale = dt / (density * mMac.cellSize());

    // Application of Equation 5.1.
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

void Projection::buildPressureMatrix(const f32 dt, const f32 density) {
    // Page 78, Figure 5.5.

    const f32 scale = dt / (density * mMac.cellSize() * mMac.cellSize());

    std::fill(mAdiag.begin(), mAdiag.end(), 0.0f);

    for (i32 iy = 0; iy < mMac.ny(); ++iy) {
        for (i32 ix = 0; ix < mMac.nx(); ++ix) {
            const Index index = iy * mMac.nx() + ix;

            // Enforce solid-wall boundaries at the edges of the viewport.
            if (ix < mMac.nx() - 1) {
                mAdiag[index] += scale;
                mAdiag[index + 1] += scale;
                mAx[index] = -scale;
            } else {
                mAx[index] = 0.0f;
            }

            if (iy < mMac.ny() - 1) {
                mAdiag[index] += scale;
                mAdiag[index + mMac.nx()] += scale;
                mAy[index] = -scale;
            } else {
                mAy[index] = 0.0f;
            }
        }
    }
}

void Projection::buildPreconditioner(const f32 tuning, const f32 safety) {
    // Page 87, Figure 5.7.
    // `tuning` is tau, `safety` is sigma.

    for (i32 iy = 0; iy < mMac.ny(); ++iy) {
        for (i32 ix = 0; ix < mMac.nx(); ++ix) {
            const Index index = iy * mMac.nx() + ix;

            f32 e = mAdiag[index];

            // Enforce solid-wall boundaries at the edges of the viewport.
            if (ix > 0) {
                const Index prev = index - 1;
                const f32 x = mAx[prev] * mPreconditioner[prev];
                const f32 y = mAy[prev] * mPreconditioner[prev];
                e = e - x * x + tuning * x * y;
            }

            if (iy > 0) {
                const Index prev = index - mMac.nx();
                const f32 x = mAx[prev] * mPreconditioner[prev];
                const f32 y = mAy[prev] * mPreconditioner[prev];
                e = e - y * y + tuning * x * y;
            }

            if (e < safety * mAdiag[index]) {
                e = mAdiag[index];
            }

            mPreconditioner[index] = 1.0f / std::sqrt(e);
        }
    }
}

void Projection::applyPreconditioner(std::vector<f32>& dst,
                                     const std::vector<f32>& a) {
    // Page 87, Figure 5.8.

    for (i32 iy = 0; iy < mMac.ny(); ++iy) {
        for (i32 ix = 0; ix < mMac.nx(); ++ix) {
            const Index index = iy * mMac.nx() + ix;

            f32 t = a[index];

            // Enforce solid-wall boundaries at the edges of the viewport.
            if (ix > 0) {
                const Index prev = index - 1;
                t -= mAx[prev] * mPreconditioner[prev] * dst[prev];
            }

            if (iy > 0) {
                const Index prev = index - mMac.nx();
                t -= mAy[prev] * mPreconditioner[prev] * dst[prev];
            }

            dst[index] = t * mPreconditioner[index];
        }
    }

    for (i32 iy = mMac.ny() - 1; iy >= 0; --iy) {
        for (i32 ix = mMac.nx() - 1; ix >= 0; --ix) {
            const Index index = iy * mMac.nx() + ix;

            f32 t = dst[index];

            // Enforce solid-wall boundaries at the edges of the viewport.
            if (ix < mMac.nx() - 1) {
                t -= mAx[index] * mPreconditioner[index] * dst[index + 1];
            }

            if (iy < mMac.ny() - 1) {
                t -= mAy[index] * mPreconditioner[index] *
                     dst[index + mMac.nx()];
            }

            dst[index] = t * mPreconditioner[index];
        }
    }
}

f32 Projection::dot(const std::vector<f32>& a,
                    const std::vector<f32>& b) const {
    f32 result = 0.0f;
    for (i32 i = 0; i < mMac.cellCount(); ++i) result += a[i] * b[i];
    return result;
}

void Projection::matmul(std::vector<f32>& dst, const std::vector<f32>& b) {
    for (i32 iy = 0; iy < mMac.ny(); ++iy) {
        for (i32 ix = 0; ix < mMac.nx(); ++ix) {
            const Index index = iy * mMac.nx() + ix;

            f32 t = mAdiag[index] * b[index];

            // Enforce solid-wall boundaries at the edges of the viewport.
            if (ix > 0)
                t += mAx[index - 1] * b[index - 1];

            if (iy > 0)
                t += mAy[index - mMac.nx()] * b[index - mMac.nx()];

            if (ix < mMac.nx() - 1)
                t += mAx[index] * b[index + 1];

            if (iy < mMac.ny() - 1)
                t += mAy[index] * b[index + mMac.nx()];

            dst[index] = t;
        }
    }
}

void Projection::scaledAdd(std::vector<f32>& dst,
                           const std::vector<f32>& a,
                           const std::vector<f32>& b,
                           const f32 s) {
    for (i32 i = 0; i < mMac.cellCount(); ++i) dst[i] = a[i] + s * b[i];
}

f32 Projection::infinityNorm(const std::vector<f32>& a) const {
    f32 max_A = 0.0f;
    for (i32 i = 0; i < mMac.cellCount(); ++i)
        max_A = std::fmax(max_A, std::fabs(a[i]));
    return max_A;
}
