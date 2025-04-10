#include "bridson_density_labelled.hpp"

int main() {
    BridsonDensityLabelled::launch<BridsonDensityLabelled>(
        800,
        600,
        "Labelled density solver (Bridson)",
        60.0f,
        "apps/bridson-density-labelled");
}
