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
#include <sstream>

#include <glm/glm.hpp>

#include <Core/Logger.hpp>
#include <Core/Base.hpp>

#include <Render/DataHolder.hpp>

namespace Elys {
    struct Vertex {
        glm::vec3 position{0.0f, 0.0f, 0.0f};
        glm::vec3 normal{0.0f, 0.0f, 0.0f};
        glm::vec2 texCoord{0.0f, 0.0f};
    };

    struct MeshPartial {
        std::vector<Vertex> mVertices{0};
        std::vector<uint32_t> mIndices{0};

        MeshPartial& Append(MeshPartial const &other) {
            auto previousSize = static_cast<uint32_t>(mVertices.size());
            mVertices.insert(mVertices.end(), other.mVertices.begin(), other.mVertices.end());

            for(auto i : other.mIndices) {
                mIndices.push_back(i + previousSize);
            }

            return *this;
        }
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
        explicit Mesh(MeshPartial& partial);

        static Mesh Plane(uint16_t width = 32, uint16_t height = 32);
        static Mesh Cube();
        static Mesh Sphere(uint16_t slice = 32, uint16_t stack = 32);

        [[nodiscard]] const std::shared_ptr<VertexArray>& VAO() const { return mVAO; }
        [[nodiscard]] size_t IndicesSize() const { return mIndices.size(); }
        [[nodiscard]] bool IsInit() const { return mInitialized; }
        [[nodiscard]] const std::vector<Vertex>& Vertices() const { return mVertices; }
        [[nodiscard]] const std::vector<uint32_t>& Indices() const { return mIndices; }
        [[nodiscard]] std::string Path() const { return mPath; }
      private:
        void ScaleToUnitAndCenter();
        void GenerateBuffers();

      private:
        std::shared_ptr<VertexArray> mVAO;

        std::vector<Vertex> mVertices;
        std::vector<uint32_t> mIndices;
        bool mInitialized = false;
        std::string mPath;

        friend class AssetLoader;
    };
} // namespace Elys

#endif // ELYS_MESH_LOADER_HPP
