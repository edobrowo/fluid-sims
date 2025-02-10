#include "mesh.hpp"

#include "util/format.hpp"

using namespace gl;

Mesh::Mesh() {
    create();
}

Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<GLushort>&& indices) {
    create();

    mDirty = true;
    mVertices = std::move(vertices);
    mIndices = std::move(indices);

    buffer();
}

Mesh::Mesh(const geometry::Mesh& other) {
    create();

    mDirty = true;

    mVertices = std::vector<Vertex>();
    mVertices.reserve(other.vertices().size());
    mIndices = std::vector<GLushort>();
    mIndices.reserve(other.indices().size());

    for (const geometry::Mesh::Vertex& v : other.vertices())
        mVertices.push_back({v.position, v.uv});
    for (const Index& i : other.indices())
        mIndices.push_back(static_cast<GLushort>(i));

    buffer();
}

Mesh::~Mesh() {
    destroy();
}

void Mesh::buffer() {
    glBindVertexArray(mVAO);
    glep();

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glep();

    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Mesh::Vertex),
                 mVertices.data(), GL_STATIC_DRAW);
    glep();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glep();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(GLushort),
                 mIndices.data(), GL_STATIC_DRAW);
    glep();

    glBindVertexArray(0);
    glep();

    mDirty = false;
}

void Mesh::render() {
    if (mDirty) {
        buffer();
    }

    glBindVertexArray(mVAO);
    glep();

    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_SHORT, 0);
    glep();

    glBindVertexArray(0);
    glep();
}

void Mesh::create() {
    glGenVertexArrays(1, &mVAO);
    glep();
    glGenBuffers(1, &mVBO);
    glep();
    glGenBuffers(1, &mEBO);
    glep();

    glBindVertexArray(mVAO);
    glep();

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glep();

    // Position.
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex),
        reinterpret_cast<GLvoid*>(offsetof(Mesh::Vertex, position)));
    glep();
    glEnableVertexAttribArray(0);
    glep();

    // Tex coords.
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex),
        reinterpret_cast<GLvoid*>(offsetof(Mesh::Vertex, uv)));
    glep();
    glEnableVertexAttribArray(1);
    glep();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glep();

    glBindVertexArray(0);
    glep();
}

void Mesh::destroy() {
    glDeleteBuffers(1, &mEBO);
    glep();
    glDeleteBuffers(1, &mVBO);
    glep();
    glDeleteBuffers(1, &mVAO);
    glep();

    mEBO = 0;
    mVBO = 0;
    mVAO = 0;
}
