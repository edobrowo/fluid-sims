#pragma once

#include "grid.hpp"
#include "mac_grid.hpp"

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
    std::vector<f32> mDiv;

    /// @brief A matrix diagonal.
    std::vector<f32> mAdiag;

    /// @brief A matrix off-diagonals.
    std::vector<f32> mAx;
    std::vector<f32> mAy;

    /// @brief Auxiliary vector.
    std::vector<f32> mAux;

    /// @brief Search vector.
    std::vector<f32> mSearch;

    /// @brief Preconditioner.
    std::vector<f32> mPreconditioner;

    /// @brief Builds the divergence vector (div).
    void buildDivergences();

    /// @brief Builds the pressure matrix.
    void buildPressureMatrix(const f32 dt, const f32 density);

    /// @brief Builds the preconditioner.
    void buildPreconditioner(const f32 tuning, const f32 safety);

    /// @brief Solves the Poisson equation for pressure projection using
    /// Conjugate Gradient iteration.
    void solvePressureEquation();

    /// @brief Applies the pressure update to the velocity field. Enforces the
    /// velocity field to be divergence-free.
    void applyPressureUpdate(const f32 dt, const f32 density);

    /// @brief Applies the preconditioner.
    void applyPreconditioner(std::vector<f32>& dst, const std::vector<f32>& b);

    /// @brief Dot product of two dynamically-sized vectors.
    f32 dot(const std::vector<f32>& a, const std::vector<f32>& b) const;

    /// @brief Multiplies the internal pressure matrix with vector b.
    void matmul(std::vector<f32>& dst, const std::vector<f32>& b);

    /// @brief Computes a + b * s.
    void scaledAdd(std::vector<f32>& dst,
                   const std::vector<f32>& a,
                   const std::vector<f32>& b,
                   const f32 s);

    /// @brief Returns maximum absolute value in vector a.
    f32 infinityNorm(const std::vector<f32>& a) const;
};
