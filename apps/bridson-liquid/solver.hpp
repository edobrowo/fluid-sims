#pragma once

#include "advection.hpp"
#include "config.hpp"
#include "extrapolation.hpp"
#include "grid.hpp"
#include "mac_grid.hpp"
#include "projection.hpp"
#include "redistancing.hpp"

class Solver {
public:
    Solver(const Config& config);
    ~Solver() = default;

    /// @brief Updates the solver by mTimestep.
    void step();

    /// @brief Retrieve a constant reference to the surface level set.
    const Grid& surface() const;

    /// @brief Retrieve a constant reference to the pressure grid.
    const Grid& pressure() const;

    /// @brief Retrieve a constant reference to the velocity u component grid.
    const Grid& u() const;

    /// @brief Retrieve a constant reference to the velocity v component grid.
    const Grid& v() const;

    /// @brief Retrieve a constant reference to the label grid.
    const LabelGrid& label() const;

private:
    /// @brief Advects the surface and velocity through the velocity grid.
    void advect();

    /// @brief Adds external forces.
    void addForces();

    /// @brief Calculates and applies the pressure necessary to make u
    /// divergence free and enforces solid wall boundary conditions.
    void project();

    /// @brief MAC grid used by this solver.
    MACGrid mMac;

    /// @brief Timestep that the solver is advanced by each step.
    f64 mTimestep;

    /// @brief Extrapolation of U.
    Extrapolation mExtrapolateU;

    /// @brief Extrapolation of V.
    Extrapolation mExtrapolateV;

    /// @brief Advection over surface level set.
    Advection mAdvectSurface;

    /// @brief Redistancing over surface.
    Redistancing mRedistanceSurface;

    /// @brief Advection over U.
    Advection mAdvectU;

    /// @brief Advection over V.
    Advection mAdvectV;

    /// @brief Pressure projection and solid boundary enforcement.
    Projection mProject;
};
