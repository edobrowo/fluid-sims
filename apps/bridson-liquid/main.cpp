#include "bridson_liquid.hpp"

int main() {
    BridsonLiquid::launch<BridsonLiquid>(
        800, 600, "LSM liquid solver (Bridson)", 60.0f, "apps/bridson-liquid");
}
