#pragma once

#include "config.hpp"
#include "grid.hpp"
#include "mac_grid.hpp"

class Solver {
public:
    Solver(const Config& config);
    ~Solver() = default;

    /// @brief Updates the solver by mTimestep.
    void step();

private:
    /// @brief Advects vector field q through the MAC grid velocity field
    ///  for the time interval given by mTimestep.
    /// @param u Velocity field.
    /// @param q Vector field at state q^n.
    /// @return Vector field at state q^{n+1}.
    Grid advect(const Grid& q) const;

    /// @brief Calculates and applies the pressure necessary to
    ///  make u divergence free and enforces solid wall boundary
    ///  conditions.
    /// @return Updated divergence-free velocity field u'.
    Grid project(const Grid& u) const;

    /// @brief MAC grid used by this solver.
    MACGrid mGrid;

    /// @brief Timestep that the solver is advanced by each step.
    f32 mTimestep;
};
