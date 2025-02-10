#include "mesh.hpp"

using namespace geometry;

const std::vector<Mesh::Vertex>& Mesh::vertices() const {
    return mVertices;
}

const std::vector<Index>& Mesh::indices() const {
    return mIndices;
}

std::vector<Mesh::Vertex>& Mesh::verticesNC() {
    return mVertices;
}

std::vector<Index>& Mesh::indicesNC() {
    return mIndices;
}
