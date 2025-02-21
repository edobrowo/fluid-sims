#pragma once

#include "util/common.hpp"

template <u32 N>
class Grid {
public:
    Grid() {
        for (Index i = 0; i < sSize; ++i) mValues[i] = 0.0f;
    }

    Grid(Grid<N>&& other) {
        mValues = std::move(other.mValues);
    }

    ~Grid() = default;

    Grid& operator=(Grid<N>&& other) {
        mValues = std::move(other.mValues);
        return *this;
    }

    f32 operator()(const Index row, const Index col) const {
        return mValues[row * sDim + col];
    }

    f32& operator()(const Index row, const Index col) {
        return mValues[row * sDim + col];
    }

    void fill(f32 value) {
        for (Index i = 0; i < sSize; ++i) mValues[i] = value;
    }

private:
    static constexpr u32 sDim = N + 2;
    static constexpr u32 sSize = sDim * sDim;
    std::array<f32, sSize> mValues;
};

template <u32 N>
struct FormatWriter<Grid<N>> {
    static void write(const Grid<N>& grid, StringBuffer& sb) {
        sb.putSafe('[');
        for (Index row = 0; row < N + 2; ++row) {
            sb.putSafe('[');
            for (Index col = 0; col < N + 2; ++col) {
                FormatWriter<f32>::write(grid(row, col), sb);
                if (col < N + 1)
                    sb.putSafe(',');
            }
            sb.putSafe(']');
            if (row < N + 1)
                sb.putSafe('\n');
        }
    }
};
