#pragma once

#include "extrapolation.hpp"
#include "grid.hpp"
#include "label_grid.hpp"

class Redistancing {
public:
    Redistancing(Grid& q, LabelGrid& label);

    ~Redistancing() = default;

    /// @brief Redistance the level set.
    void operator()();

private:
    /// @brief Grid representation of a level set.
    Grid& mQ;

    /// @brief SDF crossings.
    Grid mCrossings;

    /// @brief Label grid for extrapolation over SDF crossings.
    LabelGrid mCrossingsLabels;

    /// @brief SDF coarse initialization.
    Grid mSdfInit;

    /// @brief Smooth sign function (SSF).
    Grid mSsf;

    /// @brief Back buffer for the field grid.
    Grid mBack;
};
