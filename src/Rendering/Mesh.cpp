//
// Created by Sam on 1/29/2022.
//

#define _USE_MATH_DEFINES
#include <cmath>

#include "Rendering/Mesh.hpp"

using namespace Core::Rendering;

Mesh::Mesh(vector<Vertex> const &vertices, vector<uint32_t> const &indices,
           vector<Texture> const &textures) {
    this->mVertices = vertices;
    this->mIndices = indices;
    this->mTextures = textures;

    setupMesh();
}

Mesh::Mesh(const Mesh &copied) {
    mIndices = copied.mIndices;
    mTextures = copied.mTextures;
    mVertices = copied.mVertices;
    mInitialized = copied.mInitialized;

    VBO = copied.VBO;
    VAO = copied.VAO;
    EBO = copied.EBO;
}

Mesh::~Mesh() {
    if (mInitialized) {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    mInitialized = false;

    mVertices.clear();
    mIndices.clear();
    mTextures.clear();
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(uint32_t),
                 &mIndices[0], GL_STATIC_DRAW);

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

void Mesh::draw(Shader const &shader) const {
    if (!mInitialized) {
        std::cerr << "RENDERING::MESH::DRAW_UNINITIALIZED_ERROR" << std::endl;
        throw std::runtime_error("RENDERING::MESH::DRAW_UNINITIALIZED_ERROR");
    }

    shader.use();

    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for(unsigned int i = 0; i < mTextures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = mTextures[i].type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++);

        shader.setInt("material." + name + number, i);
        glBindTexture(GL_TEXTURE_2D, mTextures[i].id);
    }

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Mesh Mesh::Cube() {
    vector<Vertex> vertices = {
        // FRONT
        {{-1.0f, 1.0f,  1.0f}, normalize(vec3{-1.0f,  1.0f,  1.0f}), {0, 0}},
        {{ 1.0f, 1.0f,  1.0f}, normalize(vec3{ 1.0f,  1.0f,  1.0f}), {1, 0}},
        {{ 1.0f,-1.0f,  1.0f}, normalize(vec3{ 1.0f, -1.0f,  1.0f}), {1, 1}},
        {{-1.0f,-1.0f,  1.0f}, normalize(vec3{-1.0f, -1.0f,  1.0f}), {0, 1}},
        // BACK
        {{-1.0f,  1.0f, -1.0f}, normalize(vec3{-1.0f,  1.0f, -1.0f}), {0, 0}},
        {{ 1.0f,  1.0f, -1.0f}, normalize(vec3{ 1.0f,  1.0f, -1.0f}), {1, 0}},
        {{ 1.0f, -1.0f, -1.0f}, normalize(vec3{ 1.0f, -1.0f, -1.0f}), {1, 1}},
        {{-1.0f, -1.0f, -1.0f}, normalize(vec3{-1.0f, -1.0f, -1.0f}), {0, 1}},
    };
    vector<uint32_t> triangles = {
        // FRONT
        0, 1, 2,
        0, 2, 3,
        // BACK
        5, 4, 7,
        5, 7, 6,
        // LEFT
        1, 5, 6,
        1, 6, 2,
        // RIGHT
        4, 0, 3,
        4, 3, 7,
        // TOP
        4, 5, 1,
        4, 1, 0,
        // BOTTOM
        3, 2, 6,
        3, 6, 7,
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

    vector<uint32_t> triangles;
    for (unsigned int stackIt = 0; stackIt < stack - 1; ++stackIt) {
        for (unsigned int sliceIt = 0; sliceIt < slice - 1; ++sliceIt) {
            unsigned int vertexuv = stackIt + sliceIt * stack;
            unsigned int vertexUv = stackIt + 1 + sliceIt * stack;
            unsigned int vertexuV = stackIt + (sliceIt + 1) * stack;
            unsigned int vertexUV = stackIt + 1 + (sliceIt + 1) * stack;
            triangles.insert(triangles.end(), { vertexuv, vertexUv, vertexUV });
            triangles.insert(triangles.end(), { vertexuv, vertexUV, vertexuV });
        }
    }

    return Mesh{vertices, triangles, {}};
}

Mesh Mesh::Plane(uint16_t width, uint16_t height) {
    vector<Vertex> vertices(width * height);
    vector<uint32_t> triangles;
    uint32_t index = 0;

    for(uint16_t y = 0; y < height; y++) {
        for(uint16_t x = 0; x < width; x++) {
            vec3 position = {2.0f * x / (float)width - 1.0f, 0, 2.0f * y / (float)height - 1.0f};
            vec3 normal = {0.0f, 1.0f, 0.0f};
            vec2 textCoord = {(float)x / (float)width, (float)y / (float)height};

            vertices[index++] = {
                position,
                normal,
                textCoord
            };

            if (x > 0 && y > 0) {
                triangles.insert(triangles.end(), {
                    (uint32_t) (y) * width + x,
                    (uint32_t) (y-1) * width + (x - 1),
                    (uint32_t) (y) * width + (x - 1)
                });

                triangles.insert(triangles.end(), {
                    (uint32_t) (y) * width + x,
                    (uint32_t) (y-1) * width + (x - 1),
                    (uint32_t) (y-1) * width + x
                });
            }
        }
    }



    return Mesh{vertices, triangles, {}};
}
