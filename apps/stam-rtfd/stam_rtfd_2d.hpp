#pragma once

#include "math/common.hpp"
#include "math/vector.hpp"
#include "util/common.hpp"

template <class T, u32 N>
class Grid {
public:
    Grid() = default;
    ~Grid() = default;

    T operator()(const Index row, const Index col) const {
        return mValues[row * sDim + col];
    }

    T& operator()(const Index row, const Index col) {
        return mValues[row * sDim + col];
    }

    void fill(T value) {
        for (Index i = 0; i < sSize; ++i) mValues[i] = value;
    }

private:
    static constexpr u32 sDim = N + 2;
    static constexpr u32 sSize = sDim * sDim;
    T mValues[sSize];
};

// http://graphics.cs.cmu.edu/nsp/course/15-464/Fall09/papers/StamFluidforGames.pdf
template <u32 N>
class StamRTFDSolver {
public:
    static constexpr f32 h = 1.0f / static_cast<f32>(N);

    static constexpr u32 sDim = N + 2;
    static constexpr u32 sSize = sDim * sDim;

    StamRTFDSolver() {
        mVelocity.fill({0.0f, 0.0f});
        mDensity.fill(0.0f);
    }
    ~StamRTFDSolver() = default;

    void setSources(const Grid<f32, N>& sources, const f32 dt) {
        for (Index row = 0; row < sDim; ++row)
            for (Index col = 0; col < sDim; ++col)
                mDensity(row, col) += dt * sources(row, col);
    }

    void diffuse(const u32 b, const f32 diffusion, const f32 dt) {
        const f32 a = dt * diffusion * N * N;
        for (Index k = 0; k < 20; ++k) {
            for (Index row = 1; row <= sDim; ++row)
                for (Index col = 1; col <= sDim; ++col)
                    mDensity(row, col) =
                        (mDensityPrev(row, col) +
                         a * (mDensity(row, col - 1) + mDensity(row, col + 1) +
                              mDensity(row - 1, col) +
                              mDensity(row + 1, col))) /
                        (1.0f + 4.0f * a);
            setBoundary(b, dt);
        }
    }

    const Grid<f32, N>& velocity() const {
        return mVelocity;
    }

    const Grid<f32, N>& density() const {
        return mDensity;
    }

private:
    void setBoundary(const u32 b, const f32 dt) {
        //
    }

    Grid<Vector2D, N> mVelocity;
    Grid<Vector2D, N> mVelocityPrev;
    Grid<f32, N> mDensity;
    Grid<f32, N> mDensityPrev;
};
