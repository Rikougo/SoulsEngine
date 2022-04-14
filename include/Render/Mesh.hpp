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
#include <memory>

#include <glm/glm.hpp>

#include <Core/Logger.hpp>
#include <Core/Base.hpp>

#include <Render/DataHolder.hpp>
#include <Render/AABB.hpp>

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

    class Mesh {
      public:
        Mesh() = default;
        Mesh(const Mesh& other); // want to copy mesh data and generate new buffer
        Mesh(Mesh& other); // want to copy mesh data and keep same buffer, may be deleted later since it is unsafe

        static Mesh LoadOFF(std::filesystem::path &path, bool loadNormals = true);
        static Mesh Plane(uint16_t width = 32, uint16_t height = 32);
        static Mesh Cube();
        static Mesh Sphere(uint16_t slice = 32, uint16_t stack = 32);

        [[nodiscard]] const shared_ptr<VertexArray>& VAO() const { return mVAO; }
        [[nodiscard]] size_t IndicesSize() const { return mIndices.size(); }
        [[nodiscard]] bool IsInit() const { return mInitialized; }
        [[nodiscard]] const vector<Vertex>& Vertices() const { return mVertices; }
        [[nodiscard]] const vector<uint32_t>& Indices() const { return mIndices; }

        [[nodiscard]] const BoundingBox& GetAABB() const {
            if (!mBoundingBox) {
                float min = std::numeric_limits<float>::max();
                float max = std::numeric_limits<float>::min();
                for (auto v : mVertices) {
                    auto localMin = std::min({v.position.x, v.position.y, v.position.z});
                    auto localMax = std::max({v.position.x, v.position.y, v.position.z});

                    if (localMax > max) max = localMax;
                    if (localMin < min) min = localMin;
                }

                mBoundingBox = new AABB(min, max);
            }

            return *mBoundingBox;
        }
      private:
        void GenerateBuffers();

      private:
        shared_ptr<VertexArray> mVAO;

        vector<Vertex> mVertices;
        vector<uint32_t> mIndices;

        bool mInitialized = false;

        mutable BoundingBox* mBoundingBox = nullptr;
    };
} // namespace Elys

#endif // ELYS_MESH_LOADER_HPP
