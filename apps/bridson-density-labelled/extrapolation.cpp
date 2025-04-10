#include "extrapolation.hpp"

Extrapolation::Extrapolation(Grid& q, LabelGrid& label)
    : mQ(q), mBack(mQ), mLabel(label), mLabelBack(label) {
}

void Extrapolation::operator()(const LabelGrid& label) {
    mLabel = label;

    Result result = Result::Updated;
    do {
        result = step();
    } while (result == Result::Updated);
}

void Extrapolation::operator()(const u32 n, const LabelGrid& label) {
    mLabel = label;

    for (u32 i = 0; i < n; ++i) {
        static_cast<void>(step());
    }
}

Extrapolation::Result Extrapolation::step() {
    Result result = Result::FixedPoint;
    mBack.fill(0.0);

    for (i32 j = 0; j < mQ.ny(); ++j) {
        for (i32 i = 0; i < mQ.nx(); ++i) {
            if (mLabel.isEmpty(i, j)) {
                u32 neighbour_count = 0;
                f64 value = 0;

                // x neighbours.
                if (i > 0 && mLabel.isNearFluid(i - 1, j)) {
                    value += mQ(i - 1, j);
                    ++neighbour_count;
                }
                if (i < mQ.nx() - 1 && mLabel.isNearFluid(i + 1, j)) {
                    value += mQ(i + 1, j);
                    ++neighbour_count;
                }

                // y neighbours.
                if (j > 0 && mLabel.isNearFluid(i, j - 1)) {
                    value += mQ(i, j - 1);
                    ++neighbour_count;
                }
                if (j < mQ.ny() - 1 && mLabel.isNearFluid(i, j + 1)) {
                    value += mQ(i, j + 1);
                    ++neighbour_count;
                }

                if (neighbour_count > 0) {
                    mLabelBack.set(i, j, Label::Extrapolated);
                    mBack(i, j) = value / static_cast<f64>(neighbour_count);
                    result = Result::Updated;
                }
            } else {
                mBack(i, j) = mQ(i, j);
                if (i < mLabelBack.nx() && j < mLabelBack.ny())
                    mLabelBack.set(i, j, mLabel(i, j));
            }
        }
    }

    std::swap(mQ, mBack);
    std::swap(mLabel, mLabelBack);

    return result;
}
