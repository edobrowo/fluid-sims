#include "stam_density.hpp"

int main() {
    StamDensity::launch<StamDensity>(
        800, 600, "Density fluid solver (Stam)", 60.0f, "apps/stam-density");
}
