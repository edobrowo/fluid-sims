#include "bridson_density.hpp"

int main() {
    BridsonLiquid::launch<BridsonLiquid>(800,
                                         600,
                                         "Density fluid solver (Bridson)",
                                         60.0f,
                                         "apps/bridson-density");
}
