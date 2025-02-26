# Building

From the project root, run
```bash
# For running
cmake -H. -Bbuild

# For development
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -H. -Bbuild
```

Then to build,
```
cmake --build build
```

Build files will be generated in the `build` directory and binaries in the `bin` directory.
