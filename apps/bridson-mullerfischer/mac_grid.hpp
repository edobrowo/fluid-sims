#pragma once

#include "grid.hpp"
#include "math/vector.hpp"
#include "util/common.hpp"

class MACGrid {
public:
    MACGrid(const i32 rows, const i32 cols, const f32 cell_size);
    ~MACGrid() = default;

    /// @brief Number of columns in the MAC grid.
    i32 cols() const;

    /// @brief Number of rows in the MAC grid.
    i32 rows() const;

    /// @brief Number of faces holding a value along the X axis in the MAC grid.
    i32 facesX() const;

    /// @brief Number of faces holding a value along the Y axis in the MAC grid.
    i32 facesY() const;

    /// @brief Size of a cell in world space.
    f32 cellSize() const;

    /// @brief Width of the MAC grid in world space. Equal to cols() *
    /// cellSize().
    f32 width() const;

    /// @brief Height of the MAC grid in world space. Equal to rows() *
    /// cellSize().
    f32 height() const;

    /// @brief Determines whether the index pair corresponds to a cell within
    /// the (cols(), rows()) bounds of the grid.
    /// @param x Column index.
    /// @param y Row index.
    bool isCellInBounds(const i32 x, const i32 y) const;

    /// @brief Determines whether the index pair corresponds to a X-axis face
    /// within the (cols(), rows()) bounds of the grid.
    /// @param x Column index.
    /// @param y Row index.
    bool isFaceXInBounds(const i32 x, const i32 y) const;

    /// @brief Determines whether the index pair corresponds to a Y-axis face
    /// within the (cols(), rows()) bounds of the grid.
    /// @param x Column index.
    /// @param y Row index.
    bool isFaceYInBounds(const i32 x, const i32 y) const;

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

    /// @brief Determines the initial position of an imaginary particle with
    /// current position pos integrated back in time by dt using RK2.
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
    i32 mCols;
    i32 mRows;
    i32 mFacesX;
    i32 mFacesY;

    f32 mCellSize;
};
