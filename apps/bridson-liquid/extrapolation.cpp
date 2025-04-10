#include "extrapolation.hpp"

Extrapolation::Extrapolation(Grid& q, LabelGrid& label)
    : mQ(q), mLabel(label), mBack(mQ) {
}

void Extrapolation::operator()() {
    Extrapolation::Result result = Result::Updated;
    do {
        result = step();
    } while (result == Result::Updated);
}

void Extrapolation::operator()(const u32 n) {
    for (u32 i = 0; i < n; ++i) {
        static_cast<void>(step());
    }
}

Extrapolation::Result Extrapolation::step() {
    Extrapolation::Result result = Extrapolation::Result::FixedPoint;

    for (i32 j = 0; j < mQ.ny(); ++j) {
        for (i32 i = 0; i < mQ.nx(); ++i) {
            if (mLabel.isEmpty(i, j)) {
                u32 neighbour_count = 0;
                f64 value = 0;

                // x neighbours.
                if (i > 0 && mLabel.isFluid(i - 1, j)) {
                    value += mQ(i - 1, j);
                    ++neighbour_count;
                }
                if (i < mQ.nx() && mLabel.isFluid(i + 1, j)) {
                    value += mQ(i + 1, j);
                    ++neighbour_count;
                }

                // y neighbours.
                if (j > 0 && mLabel.isFluid(i, j - 1)) {
                    value += mQ(i, j - 1);
                    ++neighbour_count;
                }
                if (j < mQ.ny() && mLabel.isFluid(i, j + 1)) {
                    value += mQ(i, j + 1);
                    ++neighbour_count;
                }

                if (neighbour_count > 0) {
                    mLabel.set(i, j, Label::Extrapolated);
                    mBack(i, j) = value / static_cast<f64>(neighbour_count);
                    result = Extrapolation::Result::Updated;
                }
            } else {
                mBack(i, j) = mQ(i, j);
            }
        }
    }

    std::swap(mQ, mBack);
    return result;
}
