# Building

From the project root, run

```bash
# For usage
cmake -H. -Bbuild

# For development
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -H. -Bbuild
```

Then to build,
```
cmake --build build
```

Build files will be generated in the `build` directory and binaries in the `bin` directory.

# Running

Each binary in the `bin` directory corresponds to one of the subdirectories in the `apps` directory. Configuration options can be modified in `assets/config.json` for each.

The Bridson-based simulators are interactive:
- `Q` quits the application.
- `Space` pauses/unpauses the solver.
- `N` steps the solver.
- `R` resets the solver state.
- `D` prints the current density grid.
- `P` prints the pressure grid.
- `U` prints the velocity $U$ component grid.
- `V` prints the velocity $V$ component grid.
- `L` prints the label grid.

Setting the `"save_frames"` config option to `true` will output each frame of the solver as a PNG to a `frames` subdirectory of each application root.

# Development
The `src` directory contains common utility code used by all applications. The `apps` directory contains independent fluid simulations and all relevant code for that simulation.

# References

Bridson, R. (2007). Fluid simulation for computer graphics (SIGGRAPH 2007 Course Notes). ACM SIGGRAPH.

Bridson, R. (2015). Fluid simulation for computer graphics (2nd ed.). CRC Press. https://doi.org/10.1201/9781315266008

Chandola, R. (n.d.). Shiokaze [Fluid simulation library]. https://ryichando.graphics/shiokaze/.

Stam, J. (2003). Real-time fluid dynamics for games. In Proceedings of the Game Developer Conference. https://graphics.cs.cmu.edu/nsp/course/15-464/Spring11/papers/StamFluidforGames.pdf

Bitterli, B. (n.d.). Incremental fluids [GitHub repository]. GitHub. https://github.com/tunabrain/incremental-fluids/tree/master

Marrec, T. (n.d.). Fluid simulation [GitHub repository]. GitHub. https://github.com/tmarrec/fluid-simulation?tab=readme-ov-file
