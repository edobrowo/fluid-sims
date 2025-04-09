#pragma once

#include "grid.hpp"
#include "label_grid.hpp"

class Extrapolation {
public:
    Extrapolation(Grid& q, LabelGrid& label);

    ~Extrapolation() = default;

    /// @brief Extrapolates the grid to empty cells `n` times.
    void operator()();

private:
    Grid& mQ;
    LabelGrid& mLabel;

    Grid mBack;
};
