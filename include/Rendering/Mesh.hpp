//
// Created by Sam on 1/29/2022.
//

#ifndef ELYS_MESH_HPP
#define ELYS_MESH_HPP

#include <array>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/glm/geometric.hpp>

#include "Rendering/Shader.hpp"

using glm::ivec3;
using glm::vec2;
using glm::vec3;
using glm::normalize;

using std::array;
using std::vector;

namespace Core::Rendering {
typedef glm::vec<3, uint32_t> Triangle;

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 texCoord;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
  private:
    vector<Vertex> mVertices;
    vector<uint32_t> mIndices;
    vector<Texture> mTextures;

    unsigned int VAO, VBO, EBO;
    bool mInitialized = false;

  public:
    Mesh() = default;
    Mesh(vector<Vertex> const &vertices, vector<uint32_t> const &indices,
         vector<Texture> const &textures);
    Mesh(const Mesh &copied);
    ~Mesh();

    void draw(Shader const &shader) const;

    static Mesh Cube();
    static Mesh Sphere(uint8_t slice = 64, uint8_t stack = 64);
    static Mesh Plane(uint16_t width = 32, uint16_t height = 32);

  private:
    void setupMesh();
};
} // namespace Core::Rendering

#endif // ELYS_MESH_HPP
