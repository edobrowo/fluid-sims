#pragma once

#include "advection.hpp"
#include "config.hpp"
#include "grid.hpp"
#include "mac_grid.hpp"
#include "projection.hpp"

class Solver {
public:
    Solver(const Config& config);

    ~Solver() = default;

    /// @brief Updates the solver by mTimestep.
    void step();

    /// @brief Retrieve a constant reference to the density grid.
    const Grid& density() const;

    /// @brief Retrieve a constant reference to the pressure grid.
    const Grid& pressure() const;

    /// @brief Retrieve a constant reference to the velocity u component grid.
    const Grid& u() const;

    /// @brief Retrieve a constant reference to the velocity v component grid.
    const Grid& v() const;

private:
    /// @brief Advects density and velocity through the velocity grid.
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

    /// @brief Fluid density.
    f64 mDensity;

    /// @brief Advection over density.
    Advection mAdvectDensity;

    /// @brief Advection over U.
    Advection mAdvectU;

    /// @brief Advection over V.
    Advection mAdvectV;

    /// @brief Pressure projection and solid boundary enforcement.
    Projection mProject;
};
