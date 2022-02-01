//
// Created by Sam on 1/29/2022.
//

#define _USE_MATH_DEFINES
#include <cmath>

#include "Rendering/Mesh.hpp"

using namespace Core::Rendering;

Mesh::Mesh() {}
Mesh::Mesh(std::string objPath) {}

Mesh::~Mesh() {
    mInitialized = false;

    mVertices.clear();
    mTriangles.clear();
    mTextures.clear();

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

Mesh::Mesh(vector<Vertex> const &vertices, vector<Triangle> const &triangles,
           vector<Texture> const &textures) {
    this->mVertices = vertices;
    this->mTriangles = triangles;
    this->mTextures = textures;

    setupMesh();
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex),
                 &mVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mTriangles.size() * sizeof(Triangle),
                 &mTriangles[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, texCoord));

    glBindVertexArray(0);

    mInitialized = true;
}

void Mesh::draw(Shader const &shader) {
    if (!mInitialized)
        throw std::runtime_error("RENDERING::MESH::DRAW_UNINITIALIZED_ERROR");

    shader.use();
    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, mTriangles.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Mesh Mesh::Cube() {
    vector<Vertex> vertices = {
        // FRONT
        {{-0.5, 0.5, 0.5}, {-0.5, 0.5, 0.5}, {0, 0}},
        {{0.5, 0.5, 0.5}, {0.5, 0.5, 0.5}, {1, 0}},
        {{0.5, -0.5, 0.5}, {0.5, -0.5, 0.5}, {1, 1}},
        {{-0.5, -0.5, 0.5}, {-0.5, -0.5, 0.5}, {0, 1}},
        // BACK
        {{-0.5, 0.5, -0.5}, {-0.5, 0.5, -0.5}, {0, 0}},
        {{0.5, 0.5, -0.5}, {0.5, 0.5, -0.5}, {1, 0}},
        {{0.5, -0.5, -0.5}, {0.5, -0.5, -0.5}, {1, 1}},
        {{-0.5, -0.5, -0.5}, {-0.5, -0.5, -0.5}, {0, 1}},
    };
    vector<Triangle> triangles = {
        // FRONT
        {0, 1, 2},
        {0, 2, 3},
        // BACK
        {5, 4, 7},
        {5, 7, 6},
        // LEFT
        {1, 5, 6},
        {1, 6, 2},
        // RIGHT
        {4, 0, 3},
        {4, 3, 7},
        // TOP
        {4, 5, 1},
        {4, 1, 0},
        // BOTTOM
        {3, 2, 6},
        {3, 6, 7},
    };
    vector<Texture> textures(0);

    return Mesh{vertices, triangles, textures};
}
// TODO Improve Sphere generation code
Mesh Mesh::Sphere(uint8_t slice, uint8_t stack) {
    vector<Vertex> vertices(slice * stack);
    for (unsigned int stackIt = 0; stackIt < stack; ++stackIt) {
        float u = (float)(stackIt) / (float)(stack - 1);
        float theta = u * 2 * M_PI;
        for (unsigned int sliceIt = 0; sliceIt < slice; ++sliceIt) {
            unsigned int vertexIndex = stackIt + sliceIt * stack;
            float v = (float)(sliceIt) / (float)(slice - 1);
            float phi = -M_PI_2 + v * M_PI;
            vec3 xyz = {
                cos(theta) * cos(phi),
                sin(theta) * cos(phi),
                sin(phi)
            };
            vertices[vertexIndex] = {
                xyz,
                glm::normalize(xyz),
                {u, v}
            };
        }
    }

    vector<Triangle> triangles;
    for (unsigned int stackIt = 0; stackIt < stack; ++stackIt) {
        for (unsigned int sliceIt = 0; sliceIt < slice; ++sliceIt) {
            unsigned int vertexuv = stackIt + sliceIt * stack;
            unsigned int vertexUv = stackIt + 1 + sliceIt * stack;
            unsigned int vertexuV = stackIt + (sliceIt + 1) * stack;
            unsigned int vertexUV = stackIt + 1 + (sliceIt + 1) * stack;
            triangles.push_back(Triangle{vertexuv, vertexUv, vertexUV});
            triangles.push_back(Triangle{vertexuv, vertexUV, vertexuV});
        }
    }

    return Mesh{vertices, triangles, {}};
}
Mesh Mesh::Plane() { return Mesh(); }
