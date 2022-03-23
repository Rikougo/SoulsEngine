//
// Created by Sam on 3/22/2022.
//

#include <Render/MeshLoader.hpp>

#include <glad/glad.h>

namespace Elys {
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

            result.mVertices[NORMAL].resize(vAmount);

            // vertices info
            for (uint64_t i = 0; i < vAmount; i++) {
                float x, y, z;
                offFile >> x >> y >> z;

                // Update cube info
                auto localMin = std::min({x, y, z});
                auto localMax = std::max({x, y, z});

                if (localMax > max) max = localMax;
                if (localMin < min) min = localMin;

                result.mVertices[NORMAL][i].position = {x, y, z};

                if (loadNormals) {
                    offFile >> x >> y >> z;
                    result.mVertices[NORMAL][i].normal = {x, y, z};
                } else {
                    result.mVertices[NORMAL][i].normal = glm::normalize(vec3{x, y, z});
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

                    result.mIndices[NORMAL].insert(result.mIndices[NORMAL].end(), {a, b, c});
                } else if (size == 4) {
                    uint32_t a, b, c, d;
                    offFile >> a >> b >> c >> d;
                    result.mIndices[NORMAL].insert(result.mIndices[NORMAL].end(), {a, b, c});
                    result.mIndices[NORMAL].insert(result.mIndices[NORMAL].end(), {a, c, d});
                } else {
                    ELYS_CORE_WARN("Can't handle shape input different than 3 or 4. {0}", size);
                }

                // triangle normals aren't used yet
                if (loadNormals) offFile >> normalTrash >> normalTrash >> normalTrash;
            }

            result.GenerateBuffers(NORMAL);
            result.ComputeLOD(min, max);

            offFile.close();

            return result;
        }

        throw std::runtime_error("Cant load model at " + path.string());
    }

    Mesh Mesh::Plane(uint16_t width, uint16_t height) {
        Mesh result;
        result.mVertices[NORMAL].resize((width + 1) * (height + 1));
        uint32_t index = 0;

        std::default_random_engine generator;
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);

        for (uint16_t y = 0; y <= height; y++) {
            for (uint16_t x = 0; x <= width; x++) {

                vec3 position = {2.0f * x / (float)width - 1.0f, dist(generator), 2.0f * y / (float)height - 1.0f};
                vec3 normal = {0.0f, 1.0f, 0.0f};
                vec2 textCoord = {(float)x / (float)width, (float)y / (float)height};

                result.mVertices[NORMAL][index++] = {position, normal, textCoord};

                if (x > 0 && y > 0) {
                    result.mIndices[NORMAL].insert(result.mIndices[NORMAL].end(), {(uint32_t)(y) * (width + 1) + x,
                                                       (uint32_t)(y - 1) * (width + 1) + (x - 1),
                                                       (uint32_t)(y) * (width + 1) + (x - 1)});

                    result.mIndices[NORMAL].insert(result.mIndices[NORMAL].end(), {(uint32_t)(y) * (width + 1) + x,
                                                       (uint32_t)(y - 1) * (width + 1) + (x - 1),
                                                       (uint32_t)(y - 1) * (width + 1) + x});
                }
            }
        }

        result.GenerateBuffers(NORMAL);

        for(int level = HIGH; level < LOD_AMOUNT; level++) {
            result.mVertices[level] = result.mVertices[NORMAL];
            result.mIndices[level] = result.mIndices[NORMAL];
            result.GenerateBuffers((LODLevel)level);
        }

        return result;
    }

    void Mesh::GenerateBuffers(LODLevel level) {
        glGenVertexArrays(1, &mVAO[level]);
        glGenBuffers(1, &mVBO[level]);
        glGenBuffers(1, &mEBO[level]);

        glBindVertexArray(mVAO[level]);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO[level]);

        glBufferData(GL_ARRAY_BUFFER, mVertices[level].size() * sizeof(Vertex),
                     &mVertices[level][0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO[level]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices[level].size() * sizeof(uint32_t),
                     &mIndices[level][0], GL_STATIC_DRAW);

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
    }

    /**
     * TODO Improve compute speed, start by lower levels and store values to speed up cube contains test
     * @param min
     * @param max
     */
    void Mesh::ComputeLOD(float min, float max) {
        for (int level = HIGH; level < LOD_AMOUNT; level++) {
            ELYS_CORE_INFO("LOD... {0}", level);
            uint32_t resolution = LODResolutions[level];
            ELYS_CORE_INFO("Process LOD level of mesh on level {0} (resolution {1})", level, resolution);
            float size = max - min;
            vec3 corner{min, min, min};
            float pad = size / resolution;

            std::vector<unsigned int> indicesMap(mVertices[NORMAL].size());

            for (uint32_t i = 0; i < resolution; i++) {
                for (uint32_t j = 0; j < resolution; j++) {
                    for (uint32_t k = 0; k < resolution; k++) {
                        corner = vec3{
                            min + pad * i,
                            min + pad * j,
                            min + pad * k
                        };

                        Cube cube{corner, pad};

                        vec3 position{0, 0, 0};
                        vec3 normal{0, 0, 0};
                        vec2 texCoord{0, 0};
                        uint32_t amount = 0;

                        for(size_t l = 0; l < mVertices[NORMAL].size(); l++) {
                            Vertex v = mVertices[NORMAL][l];
                            if (cube.contains(v.position)) {
                                position += v.position;
                                normal   += v.normal;
                                texCoord += v.texCoord;

                                indicesMap[l] = mVertices[level].size();

                                amount++;
                            }
                        }

                        if (amount > 0) {
                            mVertices[level].push_back({
                                .position = position / (float)amount,
                                .normal = normal / (float)amount,
                                .texCoord = texCoord / (float)amount
                            });
                        }
                    }
                }
            }

            ELYS_CORE_INFO("Processed vertices fusion.");

            for(size_t i = 0; i < mIndices[NORMAL].size(); i+=3) {
                auto i1 = indicesMap[mIndices[NORMAL][i]], i2 = indicesMap[mIndices[NORMAL][i+1]], i3 = indicesMap[mIndices[NORMAL][i+2]];

                // save only triangles that has three different vertices
                if (i1 != i2 && i2 != i3 && i1 != i3) {
                    mIndices[level].push_back(i1);
                    mIndices[level].push_back(i2);
                    mIndices[level].push_back(i3);
                }
            }

            ELYS_CORE_INFO("Processed triangle remapping.");

            GenerateBuffers((LODLevel)level);
        }
    }
} // namespace Elys