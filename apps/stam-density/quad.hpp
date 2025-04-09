#pragma once

#include "mesh.hpp"

class Quad : public geometry::Mesh {
public:
    Quad() {
        // clang-format off
        mVertices = {
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},
            {{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}},
            {{ 0.5f,  0.5f, 0.0f}, {1.0f, 1.0f}},
        };

        mIndices = {
            0, 3, 2,
            0, 1, 3,
        };
        // clang-format on
    }

    ~Quad() = default;
};
