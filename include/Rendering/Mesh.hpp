//
// Created by Sam on 1/29/2022.
//

#ifndef ELYS_RENDERING_MESH_HPP
#define ELYS_RENDERING_MESH_HPP

#include <array>
#include <string>
#include <vector>
#include <optional>

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
using std::optional;

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

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
    vector<Vertex> mVertices{};
    vector<uint32_t> mIndices{};
    vector<Texture> mTextures{};

    optional<Texture> mHeightMap{};

    unsigned int mVAO = 0, mVBO = 0, mEBO = 0;
    bool mInitialized = false;

  public:
    /**
     * WARNING : DOEST NOT initialize buffers you may want to call Mesh::SetupMesh() before drawing mesh
     */
    Mesh() = default;

    /**
     * WARNING : DOES NOT initialize buffers you may want to call Mesh::SetupMesh() before drawing mesh
     * @param vertices
     * @param indices
     * @param textures
     */
    Mesh(vector<Vertex> const &vertices, vector<uint32_t> const &indices,
         vector<Texture> const &textures);

    /**
     * Copy constructor copy all data (vertices, indices, textures, heightmap) but DOES NOT initialize it (and it doesn't copy buffers)
     * @param copied
     */
    Mesh(const Mesh &copied);

    ~Mesh();

    /**
     * Generate buffers and vertex array object, it is important to call it once before drawing mesh
     * Calling this method on initialized Mesh will throw an error
     */
    void SetupMesh();

    /**
     * Update vertices and indices data, also update buffers if initialized
     * @param vertices
     * @param indices
     */
    void UpdateMesh(vector<Vertex> const &vertices, vector<uint32_t> const &indices);
    inline void UpdateMesh(Mesh const &mesh) { UpdateMesh(mesh.mVertices, mesh.mIndices); };

    /**
     * Draw the mesh using the given shader
     * WARING : Calling this method on uninitialized Mesh will throw an error
     * @param shader
     */
    void Draw(Shader const &shader) const;
    void SetTexture(const char* path);
    void SetTexture(Texture const &texture);

    void ConfigureHeightMap(const char* path);
    void ConfigureHeightMap(Texture const &texture);
    void RemoveHeightMap();

    static Mesh Cube();
    static Mesh Sphere(uint8_t slice = 64, uint8_t stack = 64);

    /**
     * Generate unit plane with a definition of width * height (which are uint16_t so the maximum amount
     * of vertices is 2^32 but in fact it's heavy cost beyond 512 * 512)
     * @param width how many vertices on the x axis
     * @param height how many vertices on the z axis
     * @return Corresponding Mesh
     */
    static Mesh Plane(uint16_t width = 32, uint16_t height = 32);
  private:
    void ResetMesh();
};
} // namespace Core::Rendering

#endif // ELYS_RENDERING_MESH_HPP
