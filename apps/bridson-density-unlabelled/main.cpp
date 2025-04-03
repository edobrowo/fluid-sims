#include "bridson_density_unlabelled.hpp"

int main() {
    BridsonDensity::launch<BridsonDensity>(800,
                                           600,
                                           "Bridson Density-based Unlabelled",
                                           60.0f,
                                           "apps/bridson-density-unlabelled");
}
