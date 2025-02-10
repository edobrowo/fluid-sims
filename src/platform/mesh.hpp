#pragma once

#include "geometry/mesh.hpp"
#include "math/vector.hpp"
#include "opengl.hpp"

namespace gl {

class Mesh {
public:
    struct Vertex {
        Vector3D position;
        Vector2D uv;
    };

    Mesh();
    Mesh(std::vector<Vertex>&& vertices, std::vector<GLushort>&& indices);
    Mesh(const geometry::Mesh& other);
    Mesh(const Mesh& other) = delete;
    ~Mesh();

    void buffer();
    void render();

protected:
    void create();
    void destroy();

    std::vector<Vertex> mVertices;
    std::vector<GLushort> mIndices;

private:
    GLuint mVBO;
    GLuint mVAO;
    GLuint mEBO;
    bool mDirty;
};

}
