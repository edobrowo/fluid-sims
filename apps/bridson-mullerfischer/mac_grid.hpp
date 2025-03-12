#pragma once

#include "grid.hpp"
#include "math/vector.hpp"
#include "util/common.hpp"

class MACGrid {
public:
    MACGrid(const Size rows, const Size cols, const f32 cell_size);
    ~MACGrid() = default;

    /// @brief Number of columns in the MAC grid.
    Size cols() const;

    /// @brief Number of rows in the MAC grid.
    Size rows() const;

    /// @brief Number of faces holding a value along the X axis in the MAC grid.
    Size facesX() const;

    /// @brief Number of faces holding a value along the Y axis in the MAC grid.
    Size facesY() const;

    /// @brief Size of a cell in world space.
    f32 cellSize() const;

    /// @brief Width of the MAC grid in world space. Equal to cols() *
    /// cellSize().
    f32 width() const;

    /// @brief Height of the MAC grid in world space. Equal to rows() *
    /// cellSize().
    f32 height() const;

    /// @brief Retrieves the cell position in worldspace.
    /// @param x Column index.
    /// @param y Row index.
    Vector2D cellPosition(const Index x, const Index y) const;

    /// @brief Determines whether the index pair corresponds to a cell within
    /// the (cols(), rows()) bounds of the grid.
    /// @param x Column index.
    /// @param y Row index.
    bool isCellInBounds(const Index x, const Index y) const;

    /// @brief Determines whether the index pair corresponds to a X-axis face
    /// within the (cols(), rows()) bounds of the grid.
    /// @param x Column index.
    /// @param y Row index.
    bool isFaceXInBounds(const Index x, const Index y) const;

    /// @brief Determines whether the index pair corresponds to a Y-axis face
    /// within the (cols(), rows()) bounds of the grid.
    /// @param x Column index.
    /// @param y Row index.
    bool isFaceYInBounds(const Index x, const Index y) const;

    /// @brief Retrieves the velocity at the given worldspace position.
    /// @param pos Worldspace position.
    Vector2D velocity(const Vector2D& pos) const;

    /// @brief Retrieves the pressure at the given worldspace position.
    /// @param pos Worldspace position.
    f32 pressure(const Vector2D& pos) const;

    /// @brief Retrieves the density at the given worldspace position.
    /// @param pos Worldspace position.
    f32 density(const Vector2D& pos) const;

    /// @brief Retrieves the temperature at the given worldspace position.
    /// @param pos Worldspace position.
    f32 temperature(const Vector2D& pos) const;

    /// @brief Determines the position of an imaginary particle with initial
    /// position pos at time given by the current time minus timestep dt.
    /// @param pos Worldspace position.
    /// @param dt Time step.
    Vector2D backtrace(const Vector2D& pos, const f32 dt) const;

    /// @brief Uses the CFL heuristic to determine a large timestep.
    f32 cflTimestep() const;

    /// @brief Velocity x-component.
    Grid mU;

    /// @brief Velocity y-component.
    Grid mV;

    /// @brief Pressure.
    Grid mP;

    /// @brief Density.
    Grid mD;

    /// @brief Temperature.
    Grid mT;

private:
    Size mCols;
    Size mRows;
    Size mFacesX;
    Size mFacesY;

    f32 mCellSize;
};
