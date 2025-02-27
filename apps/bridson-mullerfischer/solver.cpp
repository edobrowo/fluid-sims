#include "solver.hpp"

Solver::Solver(const Config& config)
    : mGrid(config.rows, config.cols, config.cellSize),
      mTimestep(config.timestep) {
}

void Solver::step() {
}
