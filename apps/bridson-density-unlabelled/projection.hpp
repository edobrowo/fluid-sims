#pragma once

#include "grid.hpp"
#include "mac_grid.hpp"

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
    std::vector<f64> mDiv;

    /// @brief A matrix diagonal.
    std::vector<f64> mAdiag;

    /// @brief A matrix off-diagonals.
    std::vector<f64> mAx;
    std::vector<f64> mAy;

    /// @brief Auxiliary vector.
    std::vector<f64> mAux;

    /// @brief Search vector.
    std::vector<f64> mSearch;

    /// @brief Preconditioner.
    std::vector<f64> mPreconditioner;

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
    void applyPreconditioner(std::vector<f64>& dst, const std::vector<f64>& b);

    /// @brief Dot product of two dynamically-sized vectors.
    f64 dot(const std::vector<f64>& a, const std::vector<f64>& b) const;

    /// @brief Multiplies the internal pressure matrix with vector b.
    void matmul(std::vector<f64>& dst, const std::vector<f64>& b);

    /// @brief Computes a + b * s.
    void scaledAdd(std::vector<f64>& dst,
                   const std::vector<f64>& a,
                   const std::vector<f64>& b,
                   const f64 s);

    /// @brief Returns maximum absolute value in vector a.
    f64 infinityNorm(const std::vector<f64>& a) const;
};
