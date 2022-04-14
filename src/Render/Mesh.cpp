//
// Created by Sam on 3/22/2022.
//

#include <Render/Mesh.hpp>

#include <glad/glad.h>

namespace Elys {
    Mesh::Mesh(const Mesh &other) {
        mVertices = other.mVertices;
        mIndices = other.mIndices;
        mVAO = other.mVAO;
    }
    Mesh::Mesh(Mesh &other) {
        mVertices = other.mVertices;
        mIndices = other.mIndices;
        mVAO = other.mVAO;
    }

    Mesh Mesh::LoadOFF(std::filesystem::path &path, bool loadNormals) {
        Mesh result{};

        std::ifstream offFile(path);

        if (offFile.is_open()) {
            std::string type;
            uint64_t vAmount, tAmount, nAmount;
            float min = std::numeric_limits<float>::max();
            float max = std::numeric_limits<float>::min();

            offFile >> type >> vAmount >> tAmount >> nAmount;
            ELYS_CORE_INFO("Loading OFF mesh, vertices amount : {0}, shape amount : {1}", vAmount, tAmount);

            result.mVertices.resize(vAmount);

            // vertices info
            for (uint64_t i = 0; i < vAmount; i++) {
                float x, y, z;
                offFile >> x >> y >> z;

                // Update cube info
                auto localMin = std::min({x, y, z});
                auto localMax = std::max({x, y, z});

                if (localMax > max) max = localMax;
                if (localMin < min) min = localMin;

                result.mVertices[i].position = {x, y, z};

                if (loadNormals) {
                    offFile >> x >> y >> z;
                    result.mVertices[i].normal = {x, y, z};
                } else {
                    result.mVertices[i].normal = glm::normalize(vec3{x, y, z});
                }
            }

            // triangles
            for (uint64_t i = 0; i < tAmount; i++) {
                uint32_t size;
                offFile >> size;

                float normalTrash;

                if (size == 3) {
                    uint32_t a, b, c;
                    offFile >> a >> b >> c;

                    result.mIndices.insert(result.mIndices.end(), {a, b, c});
                } else if (size == 4) {
                    uint32_t a, b, c, d;
                    offFile >> a >> b >> c >> d;
                    result.mIndices.insert(result.mIndices.end(), {a, b, c, a, c, d});
                } else {
                    ELYS_CORE_WARN("Can't handle shape input different than 3 or 4. {0}", size);
                }

                // triangle normals aren't used yet
                if (loadNormals) offFile >> normalTrash >> normalTrash >> normalTrash;
            }

            result.GenerateBuffers();

            offFile.close();

            return result;
        }

        throw std::runtime_error("Cant load model at " + path.string());
    }

    Mesh Mesh::Plane(uint16_t width, uint16_t height) {
        Mesh result;
        result.mVertices.resize((width + 1) * (height + 1));
        uint32_t index = 0;
        constexpr vec3 normal = {0.0f, 1.0f, 0.0f};

        for (uint16_t y = 0; y <= height; y++) {
            for (uint16_t x = 0; x <= width; x++) {

                vec3 position = {2.0f * x / (float)width - 1.0f, 0, 2.0f * y / (float)height - 1.0f};
                vec2 textCoord = {(float)x / (float)width, (float)y / (float)height};

                result.mVertices[index++] = {position, normal, textCoord};

                if (x > 0 && y > 0) {
                    result.mIndices.insert(result.mIndices.end(), {
                        // first triangle
                        (uint32_t)(y) * (width + 1) + x,
                        (uint32_t)(y - 1) * (width + 1) + (x - 1),
                        (uint32_t)(y) * (width + 1) + (x - 1),
                        // second triangle
                        (uint32_t)(y) * (width + 1) + x,
                        (uint32_t)(y - 1) * (width + 1) + (x - 1),
                        (uint32_t)(y - 1) * (width + 1) + x
                    });
                }
            }
        }

        result.GenerateBuffers();

        return result;
    }

    Mesh Mesh::Cube() {
        Mesh result;
        result.mVertices = {
            // FRONT
            {{-1.0f, 1.0f, 1.0f},  {0.0f, 0.0f, 1.0f}, {0, 1}},
            {{1.0f, 1.0f, 1.0f},   {0.0f, 0.0f, 1.0f}, {1, 1}},
            {{1.0f, -1.0f, 1.0f},  {0.0f, 0.0f, 1.0f}, {1, 0}},
            {{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0, 0}},
            // BACK
            {{-1.0f, 1.0f, -1.0f},  {0.0f, 0.0f, -1.0f}, {0, 1}},
            {{1.0f, 1.0f, -1.0f},   {0.0f, 0.0f, -1.0f}, {1, 1}},
            {{1.0f, -1.0f, -1.0f},  {0.0f, 0.0f, -1.0f}, {1, 0}},
            {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0, 0}},
            // LEFT
            {{-1.0f, 1.0f, 1.0f},   {-1.0f, 0.0f, 0.0f}, {0, 1}},
            {{-1.0f, 1.0f, -1.0f},  {-1.0f, 0.0f, 0.0f}, {1, 1}},
            {{-1.0f, -1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {1, 0}},
            {{-1.0f, -1.0f, 1.0f},  {-1.0f, 0.0f, 0.0f}, {0, 0}},
            // RIGHT
            {{1.0f, 1.0f, 1.0f},   {1.0f, 0.0f, 0.0f}, {0, 1}},
            {{1.0f, 1.0f, -1.0f},  {1.0f, 0.0f, 0.0f}, {1, 1}},
            {{1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1, 0}},
            {{1.0f, -1.0f, 1.0f},  {1.0f, 0.0f, 0.0f}, {0, 0}},
            // TOP
            {{-1.0f, 1.0f, 1.0f},  {0.0f, 1.0f, 0.0f}, {0, 1}},
            {{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {1, 1}},
            {{1.0f, 1.0f, -1.0f},  {0.0f, 1.0f, 0.0f}, {1, 0}},
            {{1.0f, 1.0f, 1.0f},   {0.0f, 1.0f, 0.0f}, {0, 0}},
            // BOTTOM
            {{-1.0f, -1.0f, 1.0f},  {0.0f, -1.0f, 0.0f}, {0, 1}},
            {{-1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {1, 1}},
            {{1.0f, -1.0f, -1.0f},  {0.0f, -1.0f, 0.0f}, {1, 0}},
            {{1.0f, -1.0f, 1.0f},   {0.0f, -1.0f, 0.0f}, {0, 0}},
        };

        result.mIndices = {
            // FRONT
            0, 1, 2,
            0, 2, 3,
            // BACK
            5, 4, 7,
            5, 7, 6,
            // LEFT
            8, 9, 10,
            8, 10, 11,
            // RIGHT
            12, 13, 14,
            12, 14, 15,
            // TOP
            16, 17, 18,
            16, 18, 19,
            // BOTTOM
            20, 21, 22,
            20, 22, 23,
        };

        result.GenerateBuffers();

        return result;
    }

    Mesh Mesh::Sphere(uint16_t slice, uint16_t stack) {
        Mesh result;
        result.mVertices.resize(slice * stack);
        for (unsigned int stackIt = 0; stackIt < stack; ++stackIt) {
            float u = (float)(stackIt) / (float)(stack - 1);
            float theta = u * 2 * M_PI;
            for (unsigned int sliceIt = 0; sliceIt < slice; ++sliceIt) {
                unsigned int vertexIndex = stackIt + sliceIt * stack;
                float v = (float)(sliceIt) / (float)(slice - 1);
                float phi = (float)-M_PI_2 + v * M_PI;
                vec3 xyz = {cos(theta) * cos(phi), sin(theta) * cos(phi), sin(phi)};
                result.mVertices[vertexIndex] = {xyz, glm::normalize(xyz), {u, v}};
            }
        }

        for (unsigned int stackIt = 0; stackIt < stack - 1; ++stackIt) {
            for (unsigned int sliceIt = 0; sliceIt < slice - 1; ++sliceIt) {
                uint32_t vertexuv = stackIt + sliceIt * stack;
                uint32_t vertexUv = stackIt + 1 + sliceIt * stack;
                uint32_t vertexuV = stackIt + (sliceIt + 1) * stack;
                uint32_t vertexUV = stackIt + 1 + (sliceIt + 1) * stack;
                result.mIndices.insert(result.mIndices.end(), {vertexuv, vertexUv, vertexUV, vertexuv, vertexUV, vertexuV});
            }
        }

        result.GenerateBuffers();

        return result;
    }

    void Mesh::GenerateBuffers() {
        mVAO = std::make_shared<VertexArray>();
        auto vertexBuffer = std::make_shared<VertexBuffer>((void*)&mVertices[0], mVertices.size() * sizeof(Vertex));
        BufferLayout vertexLayout{
            {"position", sizeof(vec3), 3, GL_FLOAT},
            {"normal", sizeof(vec3), 3, GL_FLOAT},
            {"texCoord", sizeof(vec2), 2, GL_FLOAT}
        };
        vertexBuffer->SetLayout(vertexLayout);
        mVAO->AddVertexBuffer(vertexBuffer);
        auto indexBuffer = std::make_shared<IndexBuffer>(&mIndices[0], mIndices.size());
        mVAO->SetIndexBuffer(indexBuffer);
        ELYS_CORE_INFO("sizeof(Vertex) : {0} | layout stride : {1}", sizeof(Vertex), vertexLayout.GetStride());

        ELYS_CORE_INFO("{0} {1} {2}", offsetof(Vertex, position), offsetof(Vertex, normal), offsetof(Vertex, texCoord));
    }
} // namespace Elys