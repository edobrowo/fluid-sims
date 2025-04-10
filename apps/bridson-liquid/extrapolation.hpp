#pragma once

#include "grid.hpp"
#include "label_grid.hpp"

class Extrapolation {
public:
    Extrapolation(Grid& q, LabelGrid& label);

    ~Extrapolation() = default;

    /// @brief Fully extrapolates the grid into empty cells.
    void operator()();

    /// @brief Extrapolates the grid `n` times.
    void operator()(const u32 n);

private:
    enum class Result {
        Updated = 0,
        FixedPoint
    };

    /// @brief Performs a single extrapolation step.
    Result step();

    Grid& mQ;
    LabelGrid& mLabel;

    Grid mBack;
};
