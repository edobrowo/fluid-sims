#include "extrapolation.hpp"

Extrapolation::Extrapolation(Grid& q, LabelGrid& label)
    : mQ(q), mLabel(label), mBack(mQ) {
}

void Extrapolation::operator()() {
    for (i32 j = 0; j < mQ.ny(); ++j) {
        for (i32 i = 0; i < mQ.nx(); ++i) {
            if (mLabel(i, j) == Label::Empty) {
                u32 neighbour_count = 0;
                f64 value = 0;

                // x neighbours.
                if (i > 0 && mLabel(i - 1, j) == Label::Fluid) {
                    value += mQ(i - 1, j);
                    ++neighbour_count;
                }
                if (i < mQ.nx() && mLabel(i + 1, j) == Label::Fluid) {
                    value += mQ(i + 1, j);
                    ++neighbour_count;
                }

                // y neighbours.
                if (j > 0 && mLabel(i, j - 1) == Label::Fluid) {
                    value += mQ(i, j - 1);
                    ++neighbour_count;
                }
                if (j < mQ.ny() && mLabel(i, j - 1) == Label::Fluid) {
                    value += mQ(i, j + 1);
                    ++neighbour_count;
                }

                if (neighbour_count > 0) {
                    mBack(i, j) = value / static_cast<f64>(neighbour_count);
                }
            } else {
                mBack(i, j) = mQ(i, j);
            }
        }
    }

    std::swap(mQ, mBack);
}
