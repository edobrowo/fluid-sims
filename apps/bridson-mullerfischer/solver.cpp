#include "solver.hpp"

namespace {

#define for_all_cells                                                          \
    for (Index y = 0; y < mGrid.rows(); ++y)                                   \
        for (Index x = 0; x < mGrid.cols(); ++x)

#define for_all_faces                                                          \
    for (Index y = 0; y < mGrid.facesY(); ++y)                                 \
        for (Index x = 0; x < mGrid.facesX(); ++x)

}

Solver::Solver(const Config& config)
    : mGrid(config.rows, config.cols, config.cellSize),
      mTimestep(config.timestep) {
}

void Solver::step() {
    //
}

Grid Solver::advect(const Grid& q) const {
    Grid q_p = q;

    for (i32 y = 0; y < q.rows(); ++y) {
        for (i32 x = 0; x < q.cols(); ++x) {
            // Determime the position corresponding to the current cell.
            const Vector2D pos = q.cellToPosition(Vector2i(x, y));

            // Integrate in time with mU.
            const Vector2D initial_pos = mGrid.backtrace(pos, mTimestep);

            // Interpolate from from the grid and set the new value.
            q_p(x, y) = q.interp(initial_pos);
        }
    }
    return q_p;
}
