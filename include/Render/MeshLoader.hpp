//
// Created by Sam on 3/22/2022.
//

#ifndef ELYS_MESH_LOADER_HPP
#define ELYS_MESH_LOADER_HPP

#include <filesystem>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>

#include <glm/glm.hpp>

#include <Core/Logger.hpp>

using glm::vec2;
using glm::vec3;
using std::vector;

namespace Elys {
    struct Vertex {
        vec3 position;
        vec3 normal;
        vec2 texCoord;
    };

    constexpr unsigned int LOD_AMOUNT = 4;

    enum LODLevel {
        NORMAL = 0,
        HIGH = 1,
        MEDIUM = 2,
        LOW = 3,
    };

    constexpr int32_t LODResolutions[LOD_AMOUNT] = {-1, 32, 16, 8};

    struct Cube {
        inline Cube() {
            m_min = vec3{};
            m_max = vec3{};
        }

        inline Cube(float min, float max) {
            m_min = vec3{min, min, min};
            m_max = vec3{max, max, max};
        }

        inline Cube(vec3 corner, float size) {
            m_min = corner;
            m_max = {corner[0] + size, corner[1] + size, corner[2] + size};
        }

        bool contains(vec3 point) {
            for (uint8_t i = 0; i < 3; i++) {
                if (point[i] > m_max[i] || point[i] < m_min[i])
                    return false;
            }

            return true;
        }

        vec3 m_min, m_max;
    };

    class Mesh {
      public:
        Mesh() = default;

        static Mesh LoadOFF(std::filesystem::path &path, bool loadNormals = true);
        static Mesh Plane(uint16_t width = 32, uint16_t height = 32);
        [[nodiscard]] unsigned int VAO(LODLevel level = NORMAL) const { return mVAO[level]; }
        [[nodiscard]] size_t IndicesSize(LODLevel level = NORMAL) const {
            return mIndices[level].size();
        }
        [[nodiscard]] bool IsInit() const { return mInitialized; }

        [[nodiscard]] vector<Vertex> Vertices() const { return mVertices[NORMAL]; }
        [[nodiscard]] vector<uint32_t> Indices() const { return mIndices[NORMAL]; }

      private:
        void GenerateBuffers(LODLevel level = NORMAL);
        void ComputeLOD(float min, float max);

      private:
        unsigned int mVAO[LOD_AMOUNT], mVBO[LOD_AMOUNT], mEBO[LOD_AMOUNT];
        vector<Vertex> mVertices[LOD_AMOUNT];
        vector<uint32_t> mIndices[LOD_AMOUNT];
        bool mInitialized = false;
    };
} // namespace Elys

#endif // ELYS_MESH_LOADER_HPP
