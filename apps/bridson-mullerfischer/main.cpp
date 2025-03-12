#include "bridson_mullerfischer.hpp"
#include "solver.hpp"

int main() {
    BridsonMullerFischer::launch<BridsonMullerFischer>(
        800, 600, "Bridson Muller-Fischer", 60.0f,
        "apps/bridson-mullerfischer");
}
