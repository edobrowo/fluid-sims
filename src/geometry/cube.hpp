#pragma once

#include "mesh.hpp"

namespace geometry {

class Cube : public Mesh {
public:
    Cube() {
        // clang-format off
        mVertices = {
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}},
            {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}},
            {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}},
            {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}}
        };

        mIndices = {
            0, 3, 2,
            0, 1, 3,
    
            1, 7, 3,
            1, 5, 7,
    
            2, 7, 6,
            2, 3, 7,
    
            4, 2, 6,
            4, 0, 2,
    
            5, 6, 7,
            5, 4, 6,
    
            0, 5, 1,
            0, 4, 5
        };
        // clang-format on
    }
    ~Cube() = default;
};

}
