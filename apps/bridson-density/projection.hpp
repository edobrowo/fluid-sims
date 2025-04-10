#pragma once

#include "grid.hpp"
#include "mac_grid.hpp"
#include "math/vectorx.hpp"

class Projection {
public:
    Projection(MACGrid& mac);

    // Projects using Conjugate Gradient with a Cholesky preconditioner.
    void operator()(const f64 dt, const f64 density);

private:
    const Size cNumberOfCGIterations = 30;

    /// @brief MAC grid. Projection acts on the pressure component.
    MACGrid& mMac;

    /// @brief Velocity divergences. RHS of the Poisson equation.
    VectorXD mDiv;

    /// @brief A matrix diagonal.
    std::vector<f64> mAdiag;

    /// @brief A matrix off-diagonals.
    std::vector<f64> mAx;
    std::vector<f64> mAy;

    /// @brief Pressure solution vector.
    VectorXD mPressure;

    /// @brief Auxiliary vector.
    VectorXD mAux;

    /// @brief Search vector.
    VectorXD mSearch;

    /// @brief Preconditioner.
    VectorXD mPreconditioner;

    /// @brief Builds the divergence vector (div).
    void buildDivergences();

    /// @brief Builds the pressure matrix.
    void buildPressureMatrix(const f64 dt, const f64 density);

    /// @brief Solves the Poisson equation for pressure projection using
    /// Conjugate Gradient iteration.
    void solvePressureEquation(const f64 tuning, const f64 safety);

    /// @brief Applies the pressure update to the velocity field. Enforces the
    /// velocity field to be divergence-free.
    void applyPressureUpdate(const f64 dt, const f64 density);

    /// @brief Builds the preconditioner.
    void buildPreconditioner(const f64 tuning, const f64 safety);

    /// @brief Applies the preconditioner.
    void applyPreconditioner(VectorXD& dst, const VectorXD& b);

    /// @brief Multiplies the internal pressure matrix with vector b.
    void applyA(VectorXD& dst, const VectorXD& b);
};
