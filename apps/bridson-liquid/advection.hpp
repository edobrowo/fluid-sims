#pragma once

#include "grid.hpp"
#include "label_grid.hpp"

class Advection {
public:
    Advection(Grid& q, Grid& u, Grid& v, LabelGrid& label);

    /// @brief Advects the grid through the specified velocity field
    ///  for the given time step, producing a new grid.
    void operator()(const f64 dt);

    /// @brief Swaps the back buffer grid with mQ. This must be a separate
    /// operation to support self-advection.
    void swap();

private:
    Grid& mQ;
    Grid& mU;
    Grid& mV;
    LabelGrid& mLabel;

    Grid mBack;

    /// @brief Determines the initial position of an imaginary particle with
    /// current gridspace position `grid_pos` integrated back in time by dt.
    Vector2D backtrace(const Vector2D& grid_pos, const f64 dt) const;

    /// @brief Integrates back in time using Euler.
    Vector2D euler(const Vector2D& grid_pos, const f64 dt) const;

    /// @brief Integrates back in time with 2nd order Runge-Kutta.
    Vector2D RK2(const Vector2D& grid_pos, const f64 dt) const;

    /// @brief Integrates back in time with 3nd order Runge-Kutta.
    Vector2D RK3(const Vector2D& grid_pos, const f64 dt) const;
};
