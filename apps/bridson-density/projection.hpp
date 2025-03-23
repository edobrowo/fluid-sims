#pragma once

#include "grid.hpp"
#include "mac_grid.hpp"

class Projection {
public:
    Projection(MACGrid& mac);

    void operator()(const f32 dt, const f32 density);

private:
    const Size cNumberOfGSIterations = 600;

    /// @brief MAC grid. Projection acts on the pressure component.
    MACGrid& mMac;

    /// @brief Velocity divergences.
    std::vector<f32> mDiv;

    /// @brief Constructs the divergence vector (div).
    void buildDivergences();

    /// @brief Solves the Poisson equation for pressure projection using
    /// Gauss-Seidel iteration.
    void solvePressureEquation(const f32 dt, const f32 density);

    /// @brief Enforces the velocity field to be divergence-free.
    void makeDivergenceFree(const f32 dt, const f32 density);
};
