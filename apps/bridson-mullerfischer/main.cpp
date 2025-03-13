#include "config.hpp"
#include "solver.hpp"
#include "util/files.hpp"

int main() {
    Config config;
    files::Json json_config =
        files::read_to_json("apps/bridson-mullerfischer/assets/config.json");
    config.rows = json_config["grid_rows"];
    config.cols = json_config["grid_cols"];
    config.cellSize = json_config["cell_size"];
    config.timestep = json_config["timestep"];

    Solver solver(config);

    solver.mGrid.mD.fill(1.0f);
    solver.mGrid.mD.fill(Vector2i(0, 0), Vector2i(3, 3), 2.0f);
    solver.mGrid.mU.fill(0.0f);
    println("U: {}", solver.mGrid.mU);
    println("D: {}", solver.mGrid.mD);

    Grid g = solver.advect(solver.mGrid.mD);
    println("D: {}", g);

    g = solver.advect(g);
    println("D: {}", g);

    g = solver.advect(g);
    println("D: {}", g);

    // BridsonMullerFischer::launch<BridsonMullerFischer>(
    //     800, 600, "Bridson Muller-Fischer", 60.0f,
    //     "apps/bridson-mullerfischer");
}
