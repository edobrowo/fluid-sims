#pragma once

#include <vector>

#include "math/vector.hpp"
#include "util/common.hpp"

namespace geometry {

class Mesh {
public:
    struct Vertex {
        Vector3F position;
        Vector2F uv;
    };

    Mesh() = default;
    ~Mesh() = default;

    const std::vector<Vertex>& vertices() const;
    const std::vector<Index>& indices() const;

    std::vector<Vertex>& verticesNC();
    std::vector<Index>& indicesNC();

protected:
    std::vector<Vertex> mVertices;
    std::vector<Index> mIndices;
};

}
