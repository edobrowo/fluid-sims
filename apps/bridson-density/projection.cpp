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
      mAux(mMac.cellCount()),
      mSearch(mMac.cellCount()),
      mPreconditioner(mMac.cellCount()) {
}

void Projection::operator()(const f32 dt, const f32 density) {
    // In general, projection subtracts the pressure gradient from the advected
    // velocity field with external forces applied and enforces the velocity
    // field to be divergence-free. Following Bridson, it also enforces
    // solid-wall boundaries.

    const f32 tau = 0.97f;
    const f32 sigma = 0.25f;

    buildDivergences();
    buildPressureMatrix(dt, density);
    solvePressureEquation(tau, sigma);
    applyPressureUpdate(dt, density);
}

void Projection::buildDivergences() {
    const f32 scale = 1.0f / mMac.cellSize();

    // Page 72, Figure 5.3.
    for (i32 j = 0; j < mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            // Equation 5.4
            const Index index = j * mMac.nx() + i;

            mDiv[index] = -scale * (mMac.u(i + 1, j) - mMac.u(i, j) +
                                    mMac.v(i, j + 1) - mMac.v(i, j));
        }
    }
}

void Projection::buildPressureMatrix(const f32 dt, const f32 density) {
    // Page 78, Figure 5.5.

    const f32 scale = dt / (density * mMac.cellSize() * mMac.cellSize());

    std::fill(mAdiag.begin(), mAdiag.end(), 0.0f);

    for (i32 j = 0; j < mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            const Index index = j * mMac.nx() + i;

            // x neighbours
            if (i < mMac.nx() - 1) {
                mAdiag[index] += scale;
                mAdiag[index + 1] += scale;
                mAx[index] = -scale;
            } else {
                mAx[index] = 0.0f;
            }

            // y neighbours
            if (j < mMac.ny() - 1) {
                mAdiag[index] += scale;
                mAdiag[index + mMac.nx()] += scale;
                mAy[index] = -scale;
            } else {
                mAy[index] = 0.0f;
            }
        }
    }
}

void Projection::solvePressureEquation(const f32 tuning, const f32 safety) {
    buildPreconditioner(tuning, safety);

    // Initial guess of zeros.
    mMac.p.fill(0.0f);

    // Tolerance for early return.
    const f32 tol = 1e-5;

    applyPreconditioner(mAux, mDiv);
    mSearch = mAux;

    if (mDiv.infinityNorm() < tol)
        return;

    f32 sigma = dot(mAux, mDiv);

    for (i32 iter = 0; iter < cNumberOfCGIterations; ++iter) {
        applyA(mAux, mSearch);

        const f32 alpha = sigma / dot(mAux, mSearch);

        // Scaled add for grid p.
        for (i32 j = 0; j < mMac.ny(); ++j) {
            for (i32 i = 0; i < mMac.nx(); ++i) {
                const Index index = j * mMac.nx() + i;
                mMac.p(i, j) = mMac.p(i, j) + alpha * mSearch[index];
            }
        }

        mDiv = mDiv + -alpha * mAux;

        if (mDiv.infinityNorm() < tol)
            return;

        applyPreconditioner(mAux, mDiv);

        const f32 sigma_new = dot(mAux, mDiv);
        const f32 beta = sigma_new / sigma;

        mSearch = mAux + beta * mSearch;
        sigma = sigma_new;
    }
}

void Projection::applyPressureUpdate(const f32 dt, const f32 density) {
    // Based on page 71, Figure 5.2.

    const f32 scale = dt / (density * mMac.cellSize());

    // Application of Equation 5.1.
    for (i32 j = 0; j < mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            const f32 p = scale * mMac.p(i, j);

            mMac.u(i, j) -= p;
            mMac.u(i + 1, j) += p;
            mMac.v(i, j) -= p;
            mMac.v(i, j + 1) += p;
        }
    }

    // Boundary conditions. Just treat the boundaries of the window as a solid
    // boundary around the fluid.
    for (i32 j = 0; j < mMac.ny(); ++j) {
        mMac.u(0, j) = 0.0f;
        mMac.u(mMac.nx(), j) = 0.0f;
    }

    for (i32 i = 0; i < mMac.nx(); ++i) {
        mMac.v(i, 0) = 0.0f;
        mMac.v(i, mMac.ny()) = 0.0f;
    }
}

void Projection::buildPreconditioner(const f32 tuning, const f32 safety) {
    // Page 87, Figure 5.7.
    // `tuning` is referred to as tau, and `safety` is referred to as sigma.

    mPreconditioner.fill(0.0f);

    for (i32 j = 0; j < mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            const Index index = j * mMac.nx() + i;

            f32 e = mAdiag[index];

            if (i > 0) {
                const Index prev = index - 1;

                const f32 x = mAx[prev] * mPreconditioner[prev];
                const f32 y = mAy[prev] * mPreconditioner[prev];

                e = e - (x * x) - tuning * (x * y);
            }

            if (j > 0) {
                const Index prev = index - mMac.nx();

                const f32 x = mAx[prev] * mPreconditioner[prev];
                const f32 y = mAy[prev] * mPreconditioner[prev];

                e = e - (y * y) - tuning * (x * y);
            }

            if (e < safety * mAdiag[index]) {
                e = mAdiag[index];
            }

            mPreconditioner[index] = 1.0f / std::sqrt(e);
        }
    }
}

void Projection::applyPreconditioner(VectorXD& dst, const VectorXD& a) {
    // Page 87, Figure 5.8.

    // First solve Lq = r.
    for (i32 j = 0; j < mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            const Index index = j * mMac.nx() + i;

            f32 t = a[index];

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

    // Next solve L^Tz = q.
    for (i32 j = mMac.ny() - 1; j >= 0; --j) {
        for (i32 i = mMac.nx() - 1; i >= 0; --i) {
            const Index index = j * mMac.nx() + i;

            f32 t = dst[index];

            // Enforce solid-wall boundaries at the edges of the viewport.
            if (i < mMac.nx() - 1) {
                const Index next = index + 1;
                t -= mAx[index] * mPreconditioner[index] * dst[next];
            }

            if (j < mMac.ny() - 1) {
                const Index next = index + mMac.nx();
                t -= mAy[index] * mPreconditioner[index] * dst[next];
            }

            dst[index] = t * mPreconditioner[index];
        }
    }
}

void Projection::applyA(VectorXD& dst, const VectorXD& b) {
    for (i32 j = 0; j < mMac.ny(); ++j) {
        for (i32 i = 0; i < mMac.nx(); ++i) {
            const Index index = j * mMac.nx() + i;

            f32 t = mAdiag[index] * b[index];

            if (i > 0) {
                const Index prev = index - 1;
                t += mAx[prev] * b[prev];
            }

            if (j > 0) {
                const Index prev = index - mMac.nx();
                t += mAy[prev] * b[prev];
            }

            if (i < mMac.nx() - 1) {
                const Index next = index + 1;
                t += mAx[index] * b[next];
            }

            if (j < mMac.ny() - 1) {
                const Index next = index + mMac.nx();
                t += mAy[index] * b[next];
            }

            dst[index] = t;
        }
    }
}

// void Projection::buildDivergences() {
//     const f32 scale = 1.0f / mMac.cellSize();

//     mDiv.resize(mFluidCount);
//     std::fill(mDiv.begin(), mDiv.end(), 0.0f);

//     // Page 72, Figure 5.3.
//     for (i32 j = 0; j < mMac.ny(); ++j) {
//         for (i32 i = 0; i < mMac.nx(); ++i) {
//             if (mMac.isFluid(i, j)) {
//                 // Equation 5.4
//                 const Index index = mFluidIndices[j * mMac.nx() + i];

//                 mDiv[index] = -scale * (mMac.u(i + 1, j) - mMac.u(i, j) +
//                                         mMac.v(i, j + 1) - mMac.v(i, j));
//             }
//         }
//     }

//     // Page 76, Figure 5.4.
//     for (i32 j = 0; j < mMac.ny(); ++j) {
//         for (i32 i = 0; i < mMac.nx(); ++i) {
//             if (mMac.isFluid(i, j)) {
//                 const Index index = mFluidIndices[j * mMac.nx() + i];

//                 if (mMac.isSolid(i - 1, j)) {
//                     mDiv[index] -= scale * mMac.u(i, j);
//                 }
//                 if (mMac.isSolid(i + 1, j)) {
//                     mDiv[index] += scale * mMac.u(i + 1, j);
//                 }

//                 if (mMac.isSolid(i, j - 1)) {
//                     mDiv[index] -= scale * mMac.v(i, j);
//                 }
//                 if (mMac.isSolid(i, j + 1)) {
//                     mDiv[index] += scale * mMac.v(i, j + 1);
//                 }
//             }
//         }
//     }
// }

// void Projection::buildPressureMatrix(const f32 dt, const f32 density) {
//     // Page 78, Figure 5.5.

//     const f32 scale = dt / (density * mMac.cellSize() * mMac.cellSize());

//     mAdiag.resize(mFluidCount);
//     std::fill(mAdiag.begin(), mAdiag.end(), 0.0f);

//     mAx.resize(mFluidCount);
//     mAy.resize(mFluidCount);

//     for (i32 j = 0; j < mMac.ny(); ++j) {
//         for (i32 i = 0; i < mMac.nx(); ++i) {
//             if (!mMac.isFluid(i, j)) {
//                 continue;
//             }

//             const Index index = mFluidIndices[j * mMac.nx() + i];

//             // x neighbours
//             if (mMac.isFluid(i - 1, j)) {
//                 mAdiag[index] += scale;
//             }

//             if (mMac.isFluid(i + 1, j)) {
//                 mAdiag[index] += scale;
//                 mAx[index] = -scale;
//             } else if (mMac.isEmpty(i + 1, j)) {
//                 mAdiag[index] += scale;
//             }

//             // y neighbours
//             if (mMac.isFluid(i, j - 1)) {
//                 mAdiag[index] += scale;
//             }

//             if (mMac.isFluid(i, j + 1)) {
//                 mAdiag[index] += scale;
//                 mAy[index] = -scale;
//             } else if (mMac.isEmpty(i, j + 1)) {
//                 mAdiag[index] += scale;
//             }
//         }
//     }
// }

// void Projection::solvePressureEquation(const f32 tuning, const f32 safety) {
//     buildPreconditioner(tuning, safety);

//     // Initial guess of zeros.
//     mMac.p.fill(0.0f);

//     // Tolerance for early return.
//     const f32 tol = 1e-5;

//     mAux.resize(mFluidCount);
//     mSearch.resize(mFluidCount);

//     applyPreconditioner(mAux, mDiv);
//     mSearch = mAux;

//     if (infinityNorm(mDiv) < tol)
//         return;

//     f32 sigma = dot(mAux, mDiv);

//     for (i32 iter = 0; iter < cNumberOfCGIterations; ++iter) {
//         matmul(mAux, mSearch);

//         const f32 alpha = sigma / dot(mAux, mSearch);

//         // Scaled add for grid p.
//         for (i32 j = 0; j < mMac.ny(); ++j) {
//             for (i32 i = 0; i < mMac.nx(); ++i) {
//                 if (mMac.isFluid(i, j)) {
//                     const Index index = mFluidIndices[j * mMac.nx() + i];
//                     mMac.p(i, j) = mMac.p(i, j) + alpha * mSearch[index];
//                 }
//             }
//         }

//         scaledAdd(mDiv, mDiv, mAux, -alpha);

//         if (infinityNorm(mDiv) < tol)
//             return;

//         applyPreconditioner(mAux, mDiv);

//         const f32 sigma_new = dot(mAux, mDiv);
//         const f32 beta = sigma_new / sigma;

//         scaledAdd(mSearch, mAux, mSearch, beta);
//         sigma = sigma_new;
//     }
// }

// void Projection::applyPressureUpdate(const f32 dt, const f32 density) {
//     // Based on page 71, Figure 5.2.

//     const f32 scale = dt / (density * mMac.cellSize());

//     // Application of Equation 5.1.
//     for (i32 j = 0; j < mMac.ny(); ++j) {
//         for (i32 i = 0; i < mMac.nx(); ++i) {
//             // Update u.
//             if (mMac.isFluid(i - 1, j) || mMac.isFluid(i, j)) {
//                 if (mMac.isSolid(i - 1, j) || mMac.isSolid(i, j)) {
//                     mMac.u(i, j) = 0.0f;
//                 } else {
//                     mMac.u(i, j) -= scale * (mMac.p(i, j) - mMac.p(i - 1,
//                     j));
//                 }
//             } else {
//                 // TODO: mark unknown
//             }

//             // Update v.
//             if (mMac.isFluid(i, j - 1) || mMac.isFluid(i, j)) {
//                 if (mMac.isSolid(i, j - 1) || mMac.isSolid(i, j)) {
//                     mMac.v(i, j) = 0.0f;
//                 } else {
//                     mMac.v(i, j) -= scale * (mMac.p(i, j) - mMac.p(i, j -
//                     1));
//                 }
//             } else {
//                 // TODO: mark unknown
//             }
//         }
//     }
// }

// void Projection::buildPreconditioner(const f32 tuning, const f32 safety) {
//     // Page 87, Figure 5.7.
//     // `tuning` is referred to as tau, and `safety` is referred to as sigma.

//     mPreconditioner.resize(mFluidCount);
//     std::fill(mPreconditioner.begin(), mPreconditioner.end(), 0.0f);

//     for (i32 j = 0; j < mMac.ny(); ++j) {
//         for (i32 i = 0; i < mMac.nx(); ++i) {
//             if (!mMac.isFluid(i, j)) {
//                 continue;
//             }

//             const Index index = mFluidIndices[j * mMac.nx() + i];

//             f32 e = mAdiag[index];

//             if (i > 0) {
//                 const Index prev = mFluidIndices[j * mMac.nx() + i - 1];

//                 const f32 x = mAx[prev] * mPreconditioner[prev];
//                 const f32 y = mAy[prev] * mPreconditioner[prev];

//                 e = e - (x * x) - tuning * (x * y);
//             }

//             if (j > 0) {
//                 const Index prev = mFluidIndices[(j - 1) * mMac.nx() + i];

//                 const f32 x = mAx[prev] * mPreconditioner[prev];
//                 const f32 y = mAy[prev] * mPreconditioner[prev];

//                 e = e - (y * y) - tuning * (x * y);
//             }

//             if (e < safety * mAdiag[index]) {
//                 e = mAdiag[index];
//             }

//             mPreconditioner[index] = 1.0f / std::sqrt(e);
//         }
//     }
// }

// void Projection::applyPreconditioner(std::vector<f32>& dst,
//                                      const std::vector<f32>& a) {
//     // Page 87, Figure 5.8.

//     // First solve Lq = r.
//     for (i32 j = 0; j < mMac.ny(); ++j) {
//         for (i32 i = 0; i < mMac.nx(); ++i) {
//             if (!mMac.isFluid(i, j)) {
//                 continue;
//             }

//             const Index index = mFluidIndices[j * mMac.nx() + i];

//             f32 t = a[index];

//             // Enforce solid-wall boundaries at the edges of the viewport.
//             if (i > 0) {
//                 const Index prev = mFluidIndices[j * mMac.nx() + i - 1];
//                 t -= mAx[prev] * mPreconditioner[prev] * dst[prev];
//             }

//             if (j > 0) {
//                 const Index prev = mFluidIndices[(j - 1) * mMac.nx() + i];
//                 t -= mAy[prev] * mPreconditioner[prev] * dst[prev];
//             }

//             dst[index] = t * mPreconditioner[index];
//         }
//     }

//     // Next solve L^Tz = q.
//     for (i32 j = mMac.ny() - 1; j >= 0; --j) {
//         for (i32 i = mMac.nx() - 1; i >= 0; --i) {
//             if (!mMac.isFluid(i, j)) {
//                 continue;
//             }

//             const Index index = mFluidIndices[j * mMac.nx() + i];

//             f32 t = dst[index];

//             // Enforce solid-wall boundaries at the edges of the viewport.
//             if (i < mMac.nx() - 1) {
//                 const Index next = mFluidIndices[j * mMac.nx() + i + 1];
//                 t -= mAx[index] * mPreconditioner[index] * dst[next];
//             }

//             if (j < mMac.ny() - 1) {
//                 const Index next = mFluidIndices[(j + 1) * mMac.nx() + i];
//                 t -= mAy[index] * mPreconditioner[index] * dst[next];
//             }

//             dst[index] = t * mPreconditioner[index];
//         }
//     }
// }

// f32 Projection::dot(const std::vector<f32>& a,
//                     const std::vector<f32>& b) const {
//     f32 result = 0.0f;
//     for (i32 i = 0; i < mFluidCount; ++i) result += a[i] * b[i];
//     return result;
// }

// void Projection::matmul(std::vector<f32>& dst, const std::vector<f32>& b) {
//     for (i32 j = 0; j < mMac.ny(); ++j) {
//         for (i32 i = 0; i < mMac.nx(); ++i) {
//             if (!mMac.isFluid(i, j)) {
//                 continue;
//             }

//             const Index index = mFluidIndices[j * mMac.nx() + i];

//             f32 t = mAdiag[index] * b[index];

//             if (i > 0 && mMac.isFluid(i - 1, j)) {
//                 const Index prev = mFluidIndices[j * mMac.nx() + i - 1];
//                 t += mAx[prev] * b[prev];
//             }

//             if (j > 0 && mMac.isFluid(i, j - 1)) {
//                 const Index prev = mFluidIndices[(j - 1) * mMac.nx() + i];
//                 t += mAy[prev] * b[prev];
//             }

//             if (i < mMac.nx() - 1 && mMac.isFluid(i + 1, j)) {
//                 const Index next = mFluidIndices[j * mMac.nx() + i + 1];
//                 t += mAx[index] * b[next];
//             }

//             if (j < mMac.ny() - 1 && mMac.isFluid(i, j + 1)) {
//                 const Index next = mFluidIndices[(j + 1) * mMac.nx() + i];
//                 t += mAy[index] * b[next];
//             }

//             dst[index] = t;
//         }
//     }
// }

// void Projection::scaledAdd(std::vector<f32>& dst,
//                            const std::vector<f32>& a,
//                            const std::vector<f32>& b,
//                            const f32 s) {
//     for (i32 i = 0; i < mFluidCount; ++i) dst[i] = a[i] + s * b[i];
// }

// f32 Projection::infinityNorm(const std::vector<f32>& a) const {
//     f32 max_A = 0.0f;
//     for (i32 i = 0; i < mFluidCount; ++i)
//         max_A = std::fmax(max_A, std::fabs(a[i]));
//     return max_A;
// }
