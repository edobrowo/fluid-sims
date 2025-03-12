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
    println("{}", mTimestep);
    Grid q_p = q;
    for_all_faces {
        println("({}, {})", x, y);
        // TODO: cell center?
        const Vector2D pos = mGrid.cellPosition(x, y);
        const Vector2D x_p = mGrid.backtrace(pos, mTimestep);
        q_p(x, y) = q.interp(x_p);
    }
    return q_p;
}

// TODO: on page 37