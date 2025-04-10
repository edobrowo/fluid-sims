#include "redistancing.hpp"

#include "math/numeric.hpp"

Redistancing::Redistancing(Grid& q, LabelGrid& label)
    : mQ(q),
      mCrossings(q),
      mCrossingsLabels(label),
      mSdfInit(q),
      mSsf(q),
      mBack(q) {
}

void Redistancing::operator()() {
    const f64 dist = 2.0;

    // Check for interface crossings.
    for (i32 j = 0; j < mQ.ny(); ++j) {
        for (i32 i = 0; i < mQ.nx(); ++i) {
            const bool x0_interface =
                i - 1 >= 0 && (mQ(i, j) >= 0) != (mQ(i - 1, j) >= 0);

            const bool x1_interface =
                i + 1 < mQ.nx() && (mQ(i, j) >= 0) != (mQ(i + 1, j) >= 0);

            const bool y0_interface =
                j - 1 >= 0 && (mQ(i, j) >= 0) != (mQ(i, j - 1) >= 0);

            const bool y1_interface =
                j + 1 < mQ.ny() && (mQ(i, j) >= 0) != (mQ(i, j + 1) >= 0);

            if (x0_interface || x1_interface || y0_interface || y1_interface) {
                mCrossings(i, j) = 1.0;
                mCrossingsLabels.set(i, j, Label::Fluid);
            } else {
                mCrossings(i, j) = 0.0;
                mCrossingsLabels.set(i, j, Label::Empty);
            }
        }
    }

    println("mCrossings\n{}", mCrossings);
    println("mCrossingsLabels\n{}", mCrossingsLabels);

    // Extrapolate the interface crossings.
    Extrapolation crossings_extrapolation(mCrossings, mCrossingsLabels);
    crossings_extrapolation(2, mCrossingsLabels);
    println("mCrossings extrapolated\n{}", mCrossings);
    println("mCrossingsLabels extrapolated\n{}", mCrossingsLabels);

    // SDF initialization.
    for (i32 j = 0; j < mQ.ny(); ++j) {
        for (i32 i = 0; i < mQ.nx(); ++i) {
            if (mCrossings(i, j) == 0.0)
                mSdfInit(i, j) = dist * math::signum_ztn(mQ(i, j));

            if (math::abs(mSdfInit(i, j)) > dist)
                mSdfInit(i, j) = dist * math::signum_ztn(mQ(i, j));
        }
    }

    // SSF construction.
    const f64 epsilon = 0.5;
    for (i32 j = 0; j < mQ.ny(); ++j) {
        for (i32 i = 0; i < mQ.nx(); ++i) {
            const f64 phi = mSdfInit(i, j);
            mSsf(i, j) = phi / (std::sqrt(math::sqr(phi) + math::sqr(epsilon)));
        }
    }

    // Iterative level set reinitialization using a semi-implicit relaxation
    // scheme to enforce the Eikonal equation.
    const f64 scale = 1.0 / mQ.cellSize();
    const u32 relax_iter_count = 8;
    for (u32 iter = 0; iter < relax_iter_count; ++iter) {
        for (i32 j = 0; j < mQ.ny(); ++j) {
            for (i32 i = 0; i < mQ.nx(); ++i) {
                if (mCrossings(i, j) == 1.0) {
                    const Vector2D grid_pos = Vector2D(i, j) + mQ.cellCenter();

                    const f64 grad_magnitude = mQ.grad(grid_pos).length();

                    mBack(i, j) =
                        (0.5 * scale * (-mSsf(i, j) * (grad_magnitude - 1.0))) +
                        mSdfInit(i, j);
                }
            }
        }
    }

    mQ = mBack;
}
