#pragma once

#include "grid.hpp"
#include "math/numeric.hpp"
#include "math/vector.hpp"
#include "util/common.hpp"
#include "util/format.hpp"

// http://graphics.cs.cmu.edu/nsp/course/15-464/Fall09/papers/StamFluidforGames.pdf
template <u32 N>
class StamRTFDSolver {
public:
    static constexpr f32 h = 1.0f / static_cast<f32>(N);

    static constexpr u32 sDim = N + 2;
    static constexpr u32 sSize = sDim * sDim;

    StamRTFDSolver()
        : mU(), mV(), mUPrev(), mVPrev(), mDensity(), mDensityPrev() {
    }
    ~StamRTFDSolver() = default;

    const Grid<N>& u() const {
        return mU;
    }

    const Grid<N>& v() const {
        return mV;
    }

    const Grid<N>& density() const {
        return mDensity;
    }

    void step(const f32 timestep, const f32 viscosity, const f32 diffusion) {
        stepVelocity(mU, mV, mUPrev, mVPrev, viscosity, timestep);
        stepDensity(mDensity, mDensityPrev, mU, mV, diffusion, timestep);
    }

    void reset() {
        mUPrev.fill(0.0f);
        mVPrev.fill(0.0f);
        mDensityPrev.fill(0.0f);
    }

    void init(u32 gauss_seidel_iterations) {
        mGaussSeidelIterations = gauss_seidel_iterations;
    }

    void addDensity(const Index row, const Index col, const f32 d) {
        if (row > sDim || col > sDim)
            return;
        mDensity(row, col) += d;
    }

    void addVelocity(const Index row, const Index col, const Vector2F& v) {
        if (row > sDim || col > sDim)
            return;
        mU(row, col) = v[0];
        mV(row, col) = v[1];
    }

private:
    void stepDensity(Grid<N>& x,
                     Grid<N>& x0,
                     Grid<N>& u,
                     Grid<N>& v,
                     const f32 diffusion,
                     const f32 dt);

    void stepVelocity(Grid<N>& u,
                      Grid<N>& v,
                      Grid<N>& u0,
                      Grid<N>& v0,
                      const f32 viscosity,
                      const f32 dt);

    void addSource(Grid<N>& x, Grid<N>& s, const f32 dt);

    void diffuse(const u32 b,
                 Grid<N>& x,
                 Grid<N>& x0,
                 const f32 diffusion,
                 const f32 dt);

    void advect(const u32 b,
                Grid<N>& d,
                Grid<N>& d0,
                Grid<N>& u,
                Grid<N>& v,
                const f32 dt);

    void project(Grid<N>& u, Grid<N>& v, Grid<N>& p, Grid<N>& div);

    void setBoundary(const u32 b, Grid<N>& x);

    Grid<N> mU;
    Grid<N> mV;
    Grid<N> mUPrev;
    Grid<N> mVPrev;
    Grid<N> mDensity;
    Grid<N> mDensityPrev;

    u32 mGaussSeidelIterations;
};

template <u32 N>
void StamRTFDSolver<N>::stepDensity(Grid<N>& x,
                                    Grid<N>& x0,
                                    Grid<N>& u,
                                    Grid<N>& v,
                                    const f32 diffusion,
                                    const f32 dt) {
    addSource(x, x0, dt);
    std::swap(x, x0);
    diffuse(0, x, x0, diffusion, dt);
    std::swap(x, x0);
    advect(0, x, x0, u, v, dt);
}

template <u32 N>
void StamRTFDSolver<N>::stepVelocity(Grid<N>& u,
                                     Grid<N>& v,
                                     Grid<N>& u0,
                                     Grid<N>& v0,
                                     const f32 viscosity,
                                     const f32 dt) {
    addSource(u, u0, dt);
    addSource(v, v0, dt);

    std::swap(u0, u);
    diffuse(1, u, u0, viscosity, dt);

    std::swap(v0, v);
    diffuse(2, v, v0, viscosity, dt);

    project(u, v, u0, v0);

    std::swap(u0, u);
    std::swap(v0, v);

    advect(1, u, u0, u0, v0, dt);
    advect(2, v, v0, u0, v0, dt);

    project(u, v, u0, v0);
}

template <u32 N>
void StamRTFDSolver<N>::addSource(Grid<N>& x, Grid<N>& s, const f32 dt) {
    for (Index row = 0; row < sDim; ++row)
        for (Index col = 0; col < sDim; ++col) x(row, col) += dt * s(row, col);
}

template <u32 N>
void StamRTFDSolver<N>::diffuse(
    const u32 b, Grid<N>& x, Grid<N>& x0, const f32 diffusion, const f32 dt) {
    const f32 a = dt * diffusion * N * N;
    for (Index k = 0; k < mGaussSeidelIterations; ++k) {
        for (Index row = 1; row <= N; ++row)
            for (Index col = 1; col <= N; ++col)
                x(row, col) =
                    (x0(row, col) + a * (x(row, col - 1) + x(row, col + 1) +
                                         x(row - 1, col) + x(row + 1, col))) /
                    (1.0f + 4.0f * a);
        setBoundary(b, x);
    }
}

template <u32 N>
void StamRTFDSolver<N>::advect(const u32 b,
                               Grid<N>& d,
                               Grid<N>& d0,
                               Grid<N>& u,
                               Grid<N>& v,
                               const f32 dt) {
    const f32 dt0 = dt * N;
    for (Index row = 1; row <= N; ++row) {
        for (Index col = 1; col <= N; ++col) {
            const f32 x = math::clamp(
                static_cast<f32>(col) - dt0 * u(row, col), 0.5f, N + 0.5f);
            const Index col0 = static_cast<Index>(x);
            const Index col1 = col0 + 1;

            const f32 y = math::clamp(
                static_cast<f32>(row) - dt0 * v(row, col), 0.5f, N + 0.5f);
            const Index row0 = static_cast<Index>(y);
            const Index row1 = row0 + 1;

            const f32 t = 1.0f - (y - static_cast<f32>(row0));
            const f32 s = 1.0f - (x - static_cast<f32>(col0));

            d(row, col) =
                math::lerp(s,
                           math::lerp(t, d0(row0, col0), d0(row1, col0)),
                           math::lerp(t, d0(row0, col1), d0(row1, col1)));
        }
    }
    setBoundary(b, d);
}

template <u32 N>
void StamRTFDSolver<N>::project(Grid<N>& u,
                                Grid<N>& v,
                                Grid<N>& p,
                                Grid<N>& div) {
    for (Index row = 1; row <= N; ++row) {
        for (Index col = 1; col <= N; ++col) {
            div(row, col) = -0.5 * h *
                            (u(row, col + 1) - u(row, col - 1) +
                             v(row + 1, col) - v(row - 1, col));
            p(row, col) = 0.0f;
        }
    }
    setBoundary(0, div);
    setBoundary(0, p);

    for (Index k = 0; k < mGaussSeidelIterations; ++k) {
        for (Index row = 1; row <= N; ++row)
            for (Index col = 1; col <= N; ++col)
                p(row, col) =
                    (div(row, col) + p(row, col - 1) + p(row, col + 1) +
                     p(row - 1, col) + p(row + 1, col)) /
                    4.0f;
        setBoundary(0, p);
    }

    for (Index row = 1; row <= N; ++row) {
        for (Index col = 1; col <= N; ++col) {
            u(row, col) -= 0.5 * (p(row, col + 1) - p(row, col - 1)) / h;
            v(row, col) -= 0.5 * (p(row + 1, col) - p(row - 1, col)) / h;
        }
    }
    setBoundary(1, u);
    setBoundary(2, v);
}

template <u32 N>
void StamRTFDSolver<N>::setBoundary(const u32 b, Grid<N>& x) {
    for (Index i = 1; i <= N; ++i) {
        x(i, 0) = b == 1 ? -x(i, 1) : x(i, 1);
        x(i, N + 1) = b == 1 ? -x(i, N) : x(i, N);
        x(0, i) = b == 2 ? -x(1, i) : x(1, i);
        x(N + 1, i) = b == 2 ? -x(N, i) : x(N, i);
    }

    x(0, 0) = 0.5 * (x(0, 1) + x(1, 0));
    x(N + 1, 0) = 0.5 * (x(N + 1, 1) + x(N, 0));
    x(0, N + 1) = 0.5 * (x(0, N) + x(1, N + 1));
    x(N + 1, N + 1) = 0.5 * (x(N + 1, N) + x(N, N + 1));
}
