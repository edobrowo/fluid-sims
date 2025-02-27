#include "bridson_mullerfischer.hpp"
#include "grid.hpp"
#include "util/format.hpp"

int main() {
    BridsonMullerFischer::launch<BridsonMullerFischer>(
        800, 600, "Bridson Muller-Fischer", 60.0f,
        "apps/bridson-mullerfischer");
}
