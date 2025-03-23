#pragma once

#include "grid.hpp"

class Advection {
public:
    Advection(Grid& q, Grid& u, Grid& v);

    /// @brief Advects the grid through the specified velocity field
    ///  for the given time step, producing a new grid.
    /// @param u Velocity field along the x axis.
    /// @param v Velocity field along the y axis.
    /// @return Grid at the updated state.
    void operator()(const f32 dt);

    /// @brief Swaps the back buffer grid with mQ. Necessary self-advection.
    void swap();

private:
    Grid& mQ;
    Grid& mU;
    Grid& mV;
    Grid mBack;

    /// @brief Determines the initial position of an imaginary particle with
    /// current position pos integrated back in time by dt.
    Vector2D backtrace(const Vector2D& grid_pos, const f32 dt) const;

    /// @brief Integrates back in time using Euler.
    Vector2D euler(const Vector2D& grid_pos, const f32 dt) const;

    /// @brief Integrates back in time with 2nd order Runge-Kutta.
    Vector2D RK2(const Vector2D& grid_pos, const f32 dt) const;

    /// @brief Integrates back in time with 3nd order Runge-Kutta.
    Vector2D RK3(const Vector2D& grid_pos, const f32 dt) const;
};
