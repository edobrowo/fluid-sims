#include "projection.hpp"

#include <algorithm>

Projection::Projection(MACGrid& mac)
    : mMac(mac),
      mDiv(mMac.cellCount()),
      mAdiag(mMac.cellCount()),
      mAx(mMac.cellCount()),
      mAy(mMac.cellCount()),
      mFluidIndices(mMac.cellCount()),
      mFluidCount(0),
      mPressure(mMac.cellCount()),
      mAux(mMac.cellCount()),
      mSearch(mMac.cellCount()),
      mPreconditioner(mMac.cellCount()) {
}

void Projection::operator()(const f64 dt, const f64 density) {
    // In general, projection subtracts the pressure gradient from the advected
    // velocity field with external forces applied and enforces the velocity
    // field to be divergence-free. Following Bridson, it also enforces
    // solid-wall boundaries.

    const f64 tau = 0.97;
    const f64 sigma = 0.25;

    indexFluidCells();
    buildDivergences();
    buildPressureMatrix(dt, density);
    solvePressureEquation(tau, sigma);

    // Populate pressure grid with pressure solutions.
    for (i32 j = 0; j < mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            if (mMac.isFluid(i, j)) {
                const Index index = mFluidIndices[j * mMac.nx() + i];
                mMac.p(i, j) = mPressure[index];
            }
        }
    }

    applyPressureUpdate(dt, density);
}

void Projection::indexFluidCells() {
    mFluidCount = 0;
    std::fill(mFluidIndices.begin(), mFluidIndices.end(), -1);
    for (i32 j = 0; j < mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            if (mMac.isFluid(i, j)) {
                const Index index = j * mMac.nx() + i;
                mFluidIndices[index] = mFluidCount;
                ++mFluidCount;
            }
        }
    }
}

void Projection::buildDivergences() {
    const f64 scale = 1.0 / mMac.cellSize();

    mDiv.resize(mFluidCount);
    mDiv.fill(0.0);

    // Page 72, Figure 5.3.
    for (i32 j = 0; j < mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            if (mMac.isFluid(i, j)) {
                // Equation 5.4
                const Index index = mFluidIndices[j * mMac.nx() + i];

                mDiv[index] = -scale * (mMac.u(i + 1, j) - mMac.u(i, j) +
                                        mMac.v(i, j + 1) - mMac.v(i, j));
            }
        }
    }

    // Page 76, Figure 5.4.
    for (i32 j = 0; j < mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            if (mMac.isFluid(i, j)) {
                const Index index = mFluidIndices[j * mMac.nx() + i];

                if (mMac.isSolid(i - 1, j)) {
                    mDiv[index] -= scale * mMac.u(i, j);
                }
                if (mMac.isSolid(i + 1, j)) {
                    mDiv[index] += scale * mMac.u(i + 1, j);
                }

                if (mMac.isSolid(i, j - 1)) {
                    mDiv[index] -= scale * mMac.v(i, j);
                }
                if (mMac.isSolid(i, j + 1)) {
                    mDiv[index] += scale * mMac.v(i, j + 1);
                }
            }
        }
    }
}

void Projection::buildPressureMatrix(const f64 dt, const f64 density) {
    // Page 78, Figure 5.5.

    const f64 scale = dt / (density * mMac.cellSize() * mMac.cellSize());

    mAdiag.resize(mFluidCount);
    std::fill(mAdiag.begin(), mAdiag.end(), 0.0);

    mAx.resize(mFluidCount);
    std::fill(mAx.begin(), mAx.end(), 0.0);

    mAy.resize(mFluidCount);
    std::fill(mAy.begin(), mAy.end(), 0.0);

    for (i32 j = 0; j < mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            if (mMac.isFluid(i, j)) {
                const Index index = mFluidIndices[j * mMac.nx() + i];

                // x neighbours
                if (mMac.isFluid(i - 1, j)) {
                    mAdiag[index] += scale;
                }

                if (mMac.isFluid(i + 1, j)) {
                    mAdiag[index] += scale;
                    mAx[index] = -scale;
                } else if (mMac.isEmpty(i + 1, j)) {
                    mAdiag[index] += scale;
                }

                // y neighbours
                if (mMac.isFluid(i, j - 1)) {
                    mAdiag[index] += scale;
                }

                if (mMac.isFluid(i, j + 1)) {
                    mAdiag[index] += scale;
                    mAy[index] = -scale;
                } else if (mMac.isEmpty(i, j + 1)) {
                    mAdiag[index] += scale;
                }
            }
        }
    }
}

void Projection::solvePressureEquation(const f64 tuning, const f64 safety) {
    buildPreconditioner(tuning, safety);

    mPressure.resize(mFluidCount);
    mAux.resize(mFluidCount);
    mSearch.resize(mFluidCount);

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
            // Update u.
            if (mMac.isFluid(i - 1, j) || mMac.isFluid(i, j)) {
                if (mMac.isSolid(i - 1, j) || mMac.isSolid(i, j)) {
                    mMac.u(i, j) = 0.0;
                } else {
                    mMac.u(i, j) -= scale * (mMac.p(i, j) - mMac.p(i - 1, j));
                }
            } else {
                // TODO: mark unknown
            }

            // Update v.
            if (mMac.isFluid(i, j - 1) || mMac.isFluid(i, j)) {
                if (mMac.isSolid(i, j - 1) || mMac.isSolid(i, j)) {
                    mMac.v(i, j) = 0.0;
                } else {
                    mMac.v(i, j) -= scale * (mMac.p(i, j) - mMac.p(i, j - 1));
                }
            } else {
                // TODO: mark unknown
            }
        }
    }
}

void Projection::buildPreconditioner(const f64 tuning, const f64 safety) {
    // Page 87, Figure 5.7.
    // `tuning` is referred to as tau, and `safety` is referred to as sigma.

    mPreconditioner.resize(mFluidCount);
    mPreconditioner.fill(0.0);

    for (i32 j = 0; j < mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            if (mMac.isFluid(i, j)) {
                const Index index = mFluidIndices[j * mMac.nx() + i];

                f64 e = mAdiag[index];

                if (i > 0) {
                    const Index prev = mFluidIndices[j * mMac.nx() + (i - 1)];

                    const f64 x = mAx[prev] * mPreconditioner[prev];
                    const f64 y = mAy[prev] * mPreconditioner[prev];

                    e = e - (x * x) - tuning * (x * y);
                }

                if (j > 0) {
                    const Index prev = mFluidIndices[(j - 1) * mMac.nx() + i];

                    const f64 x = mAx[prev] * mPreconditioner[prev];
                    const f64 y = mAy[prev] * mPreconditioner[prev];

                    e = e - (y * y) - tuning * (x * y);
                }

                if (e < safety * mAdiag[index]) {
                    e = mAdiag[index];
                }

                mPreconditioner[index] = 1.0 / std::sqrt(e);
            }
        }
    }
}

void Projection::applyPreconditioner(VectorXD& dst, const VectorXD& a) {
    // Page 87, Figure 5.8.

    // First solve Lq = r.
    for (i32 j = 0; j < mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            if (mMac.isFluid(i, j)) {
                const Index index = mFluidIndices[j * mMac.nx() + i];

                f64 t = a[index];

                // Enforce solid-wall boundaries at the edges of the
                // viewport
                if (i > 0 && mMac.isFluid(i - 1, j)) {
                    const Index prev = mFluidIndices[j * mMac.nx() + (i - 1)];
                    t -= mAx[prev] * mPreconditioner[prev] * dst[prev];
                }

                if (j > 0 && mMac.isFluid(i, j - 1)) {
                    const Index prev = mFluidIndices[(j - 1) * mMac.nx() + i];
                    t -= mAy[prev] * mPreconditioner[prev] * dst[prev];
                }

                dst[index] = t * mPreconditioner[index];
            }
        }
    }

    // Next solve L^Tz = q.
    for (i32 j = mMac.ny() - 1; j >= 0; --j) {
        for (i32 i = mMac.nx() - 1; i >= 0; --i) {
            if (mMac.isFluid(i, j)) {
                const Index index = mFluidIndices[j * mMac.nx() + i];

                f64 t = dst[index];

                // Enforce solid-wall boundaries at the edges of the
                // viewport.
                if (i < mMac.nx() - 1 && mMac.isFluid(i + 1, j)) {
                    const Index next = mFluidIndices[j * mMac.nx() + (i + 1)];
                    t -= mAx[index] * mPreconditioner[index] * dst[next];
                }

                if (j < mMac.ny() - 1 && mMac.isFluid(i, j + 1)) {
                    const Index next = mFluidIndices[(j + 1) * mMac.nx() + i];
                    t -= mAy[index] * mPreconditioner[index] * dst[next];
                }

                dst[index] = t * mPreconditioner[index];
            }
        }
    }
}

void Projection::applyA(VectorXD& dst, const VectorXD& b) {
    for (i32 j = 0; j < mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            if (mMac.isFluid(i, j)) {
                const Index index = mFluidIndices[j * mMac.nx() + i];

                f64 t = mAdiag[index] * b[index];

                if (i > 0 && mMac.isFluid(i - 1, j)) {
                    const Index prev = mFluidIndices[j * mMac.nx() + (i - 1)];
                    t += mAx[prev] * b[prev];
                }

                if (j > 0 && mMac.isFluid(i, j - 1)) {
                    const Index prev = mFluidIndices[(j - 1) * mMac.nx() + i];
                    t += mAy[prev] * b[prev];
                }

                if (i < mMac.nx() - 1 && mMac.isFluid(i + 1, j)) {
                    const Index next = mFluidIndices[j * mMac.nx() + (i + 1)];
                    t += mAx[index] * b[next];
                }

                if (j < mMac.ny() - 1 && mMac.isFluid(i, j + 1)) {
                    const Index next = mFluidIndices[(j + 1) * mMac.nx() + i];
                    t += mAy[index] * b[next];
                }

                dst[index] = t;
            }
        }
    }
}
