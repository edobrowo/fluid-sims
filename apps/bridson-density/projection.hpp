#pragma once

#include "grid.hpp"
#include "mac_grid.hpp"
#include "math/vectorx.hpp"

class Projection {
public:
    Projection(MACGrid& mac);

    // Projects using Conjugate Gradient with a Cholesky preconditioner.
    void operator()(const f32 dt, const f32 density);

private:
    const Size cNumberOfCGIterations = 30;

    /// @brief MAC grid. Projection acts on the pressure component.
    MACGrid& mMac;

    /// @brief Velocity divergences. RHS of the Poisson equation.
    VectorXD mDiv;

    /// @brief A matrix diagonal.
    std::vector<f32> mAdiag;

    /// @brief A matrix off-diagonals.
    std::vector<f32> mAx;
    std::vector<f32> mAy;

    /// @brief Fluid cell indices for sparse grid access.
    std::vector<f32> mFluidIndices;
    i32 mFluidCount;

    /// @brief Auxiliary vector.
    VectorXD mAux;

    /// @brief Search vector.
    VectorXD mSearch;

    /// @brief Preconditioner.
    VectorXD mPreconditioner;

    /// @brief Associates an index with every fluid cell.
    void indexFluidCells();

    /// @brief Builds the divergence vector (div).
    void buildDivergences();

    /// @brief Builds the pressure matrix.
    void buildPressureMatrix(const f32 dt, const f32 density);

    /// @brief Solves the Poisson equation for pressure projection using
    /// Conjugate Gradient iteration.
    void solvePressureEquation(const f32 tuning, const f32 safety);

    /// @brief Applies the pressure update to the velocity field. Enforces the
    /// velocity field to be divergence-free.
    void applyPressureUpdate(const f32 dt, const f32 density);

    /// @brief Builds the preconditioner.
    void buildPreconditioner(const f32 tuning, const f32 safety);

    /// @brief Applies the preconditioner.
    void applyPreconditioner(VectorXD& dst, const VectorXD& b);

    /// @brief Multiplies the internal pressure matrix with vector b.
    void applyA(VectorXD& dst, const VectorXD& b);
};
