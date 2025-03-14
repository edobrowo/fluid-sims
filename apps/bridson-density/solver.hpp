#pragma once

#include "config.hpp"
#include "grid.hpp"
#include "mac_grid.hpp"

class Solver {
public:
    Solver(const Config& config);
    ~Solver() = default;

    /// @brief Retrieves the color at the specified cell.
    f32 color(const Vector2i& cell) const;

    /// @brief Updates the solver by mTimestep.
    void step();

    /// @brief Adds density to the specified region.
    void addDensity(const Vector2D& pos, const Vector2D& size, const f32 d);

    /// @brief Adds velocity to the specified region.
    void addVelocity(const Vector2D& pos, const Vector2D& size,
                     const Vector2D& u);

private:
    /// @brief Calculates and applies the pressure necessary to
    ///  make mU divergence free and enforces solid wall boundary
    ///  conditions.
    /// @return Updated divergence-free velocity field.
    void project();

    /// @brief Constructs the divergence vector (mDiv).
    void buildDivergences();

    /// @brief Enforces the velocity field to be divergence-free.
    void makeDivergenceFree();

    /// @brief MAC grid used by this solver.
    MACGrid mMac;

    /// @brief Timestep that the solver is advanced by each step.
    f32 mTimestep;

    /// @brief Fluid density.
    f32 mDensity;
};
