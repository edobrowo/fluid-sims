#pragma once

#include "advection.hpp"
#include "config.hpp"
#include "extrapolation.hpp"
#include "grid.hpp"
#include "mac_grid.hpp"
#include "projection.hpp"

class Solver {
public:
    Solver(const Config& config);
    ~Solver() = default;

    /// @brief Retrieves the color at the specified cell.
    f64 color(const Vector2i& cell) const;

    /// @brief Updates the solver by mTimestep.
    void step();

private:
    /// @brief Advects density and velocity through the current velocity grid.
    void advect();

    /// @brief Adds external forces (density and velocity).
    void addForces(const Vector2D& pos,
                   const Vector2D& size,
                   const f64 d,
                   const Vector2D& u);

    /// @brief Calculates and applies the pressure necessary to
    ///  make u divergence free and enforces solid wall boundary
    ///  conditions.
    void project();

    /// @brief MAC grid used by this solver.
    MACGrid mMac;

    /// @brief Timestep that the solver is advanced by each step.
    f64 mTimestep;

    /// @brief Fluid density.
    f64 mDensity;

    /// @brief Extrapolation of U.
    Extrapolation mExtrapolateU;

    /// @brief Extrapolation of V.
    Extrapolation mExtrapolateV;

    /// @brief Advection over density.
    Advection mAdvectDensity;

    /// @brief Advection over U.
    Advection mAdvectU;

    /// @brief Advection over V.
    Advection mAdvectV;

    /// @brief Pressure projection and solid boundary enforcement.
    Projection mProject;
};
