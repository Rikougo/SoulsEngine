//
// Created by Sam on 1/29/2022.
//

#define _USE_MATH_DEFINES
#include <cmath>
#include <stb_image.h>

#include "Rendering/Mesh.hpp"

using namespace Core::Rendering;

Mesh::Mesh(vector<Vertex> const &vertices, vector<uint32_t> const &indices,
           vector<Texture> const &textures) {
    this->mVertices = vertices;
    this->mIndices = indices;
    this->mTextures = textures;
}

Mesh::Mesh(const Mesh &copied) {
    mIndices = copied.mIndices;
    mTextures = copied.mTextures;
    mVertices = copied.mVertices;
    mHeightMap = copied.mHeightMap;

    setupMesh();
}

Mesh::~Mesh() {
    resetMesh();

    mTextures.clear();
}

void Mesh::setupMesh() {
    std::cout << "Init mesh" << std::endl;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(uint32_t), &mIndices[0],
                 GL_STATIC_DRAW);

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

void Mesh::resetMesh() {
    if (mInitialized) {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    mInitialized = false;

    mVertices.clear();
    mIndices.clear();
}

void Mesh::updateMesh(const vector<Vertex> &vertices, const vector<uint32_t> &indices) {
    mVertices = vertices;
    mIndices = indices;

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(uint32_t), &mIndices[0],
                 GL_STATIC_DRAW);

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

void Mesh::updateMesh(const Mesh &mesh) {
    updateMesh(mesh.mVertices, mesh.mIndices);
}

void Mesh::draw(Shader const &shader) const {
    if (!mInitialized) {
        std::cerr << "RENDERING::MESH::DRAW_UNINITIALIZED_ERROR" << std::endl;
        throw std::runtime_error("RENDERING::MESH::DRAW_UNINITIALIZED_ERROR");
    }

    shader.use();

    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (unsigned int i = 0; i < mTextures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = mTextures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++);

        shader.setInt(name + number, i);
        glBindTexture(GL_TEXTURE_2D, mTextures[i].id);
    }

    shader.setBool("useHeightmap", mHeightMap.has_value());
    if (mHeightMap.has_value()) {
        glActiveTexture(GL_TEXTURE0 + mTextures.size());
        shader.setInt("heightmap", mTextures.size());
        glBindTexture(GL_TEXTURE_2D, mHeightMap->id);
    }

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::setTexture(const char *path) {
    Texture texture;
    texture.id = TextureFromFile(path, "./");
    texture.type = "texture_diffuse";
    texture.path = path;

    mTextures.push_back(texture);
}

void Mesh::setTexture(Texture const &texture) { mTextures.push_back(texture); }

void Mesh::ConfigureHeightMap(const char *path) {
    Texture texture;
    texture.id = TextureFromFile(path, "./");
    texture.type = "heightmap";
    texture.path = path;

    mHeightMap = std::make_optional(texture);
}

void Mesh::ConfigureHeightMap(Texture const &texture) { mHeightMap = std::make_optional(texture); }

void Mesh::RemoveHeightMap() { mHeightMap = std::nullopt; }

Mesh Mesh::Cube() {
    vector<Vertex> vertices = {
        // FRONT
        {{-1.0f, 1.0f, 1.0f}, normalize(vec3{-1.0f, 1.0f, 1.0f}), {0, 0}},
        {{1.0f, 1.0f, 1.0f}, normalize(vec3{1.0f, 1.0f, 1.0f}), {1, 0}},
        {{1.0f, -1.0f, 1.0f}, normalize(vec3{1.0f, -1.0f, 1.0f}), {1, 1}},
        {{-1.0f, -1.0f, 1.0f}, normalize(vec3{-1.0f, -1.0f, 1.0f}), {0, 1}},
        // BACK
        {{-1.0f, 1.0f, -1.0f}, normalize(vec3{-1.0f, 1.0f, -1.0f}), {0, 0}},
        {{1.0f, 1.0f, -1.0f}, normalize(vec3{1.0f, 1.0f, -1.0f}), {1, 0}},
        {{1.0f, -1.0f, -1.0f}, normalize(vec3{1.0f, -1.0f, -1.0f}), {1, 1}},
        {{-1.0f, -1.0f, -1.0f}, normalize(vec3{-1.0f, -1.0f, -1.0f}), {0, 1}},
        // LEFT
        {{-1.0f, 1.0f, 1.0f}, normalize(vec3{-1.0f, 1.0f, 1.0f}), {0, 0}},
        {{-1.0f, 1.0f, -1.0f}, normalize(vec3{1.0f, 1.0f, 1.0f}), {1, 0}},
        {{-1.0f, -1.0f, -1.0f}, normalize(vec3{1.0f, -1.0f, 1.0f}), {1, 1}},
        {{-1.0f, -1.0f, 1.0f}, normalize(vec3{-1.0f, -1.0f, 1.0f}), {0, 1}},
        // RIGHT
        {{1.0f, 1.0f, 1.0f}, normalize(vec3{-1.0f, 1.0f, -1.0f}), {0, 0}},
        {{1.0f, 1.0f, -1.0f}, normalize(vec3{1.0f, 1.0f, -1.0f}), {1, 0}},
        {{1.0f, -1.0f, -1.0f}, normalize(vec3{-1.0f, -1.0f, -1.0f}), {1, 1}},
        {{1.0f, -1.0f, 1.0f}, normalize(vec3{1.0f, -1.0f, -1.0f}), {0, 1}},
        // TOP
        {{-1.0f, 1.0f, 1.0f}, normalize(vec3{-1.0f, 1.0f, 1.0f}), {0, 0}},
        {{-1.0f, 1.0f, -1.0f}, normalize(vec3{1.0f, 1.0f, 1.0f}), {1, 0}},
        {{1.0f, 1.0f, -1.0f}, normalize(vec3{-1.0f, -1.0f, 1.0f}), {1, 1}},
        {{1.0f, 1.0f, 1.0f}, normalize(vec3{1.0f, -1.0f, 1.0f}), {0, 1}},
        // BOTTOM
        {{-1.0f, -1.0f, 1.0f}, normalize(vec3{-1.0f, 1.0f, 1.0f}), {0, 0}},
        {{-1.0f, -1.0f, -1.0f}, normalize(vec3{1.0f, 1.0f, 1.0f}), {1, 0}},
        {{1.0f, -1.0f, -1.0f}, normalize(vec3{-1.0f, -1.0f, 1.0f}), {1, 1}},
        {{1.0f, -1.0f, 1.0f}, normalize(vec3{1.0f, -1.0f, 1.0f}), {0, 1}},
    };
    vector<uint32_t> triangles = {
        // FRONT
        0,
        1,
        2,
        0,
        2,
        3,
        // BACK
        5,
        4,
        7,
        5,
        7,
        6,
        // LEFT
        8,
        9,
        10,
        8,
        10,
        11,
        // RIGHT
        12,
        13,
        14,
        12,
        14,
        15,
        // TOP
        16,
        17,
        18,
        16,
        18,
        19,
        // BOTTOM
        20,
        21,
        22,
        20,
        22,
        23,
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
            vec3 xyz = {cos(theta) * cos(phi), sin(theta) * cos(phi), sin(phi)};
            vertices[vertexIndex] = {xyz, glm::normalize(xyz), {u, v}};
        }
    }

    vector<uint32_t> triangles;
    for (unsigned int stackIt = 0; stackIt < stack - 1; ++stackIt) {
        for (unsigned int sliceIt = 0; sliceIt < slice - 1; ++sliceIt) {
            unsigned int vertexuv = stackIt + sliceIt * stack;
            unsigned int vertexUv = stackIt + 1 + sliceIt * stack;
            unsigned int vertexuV = stackIt + (sliceIt + 1) * stack;
            unsigned int vertexUV = stackIt + 1 + (sliceIt + 1) * stack;
            triangles.insert(triangles.end(), {vertexuv, vertexUv, vertexUV});
            triangles.insert(triangles.end(), {vertexuv, vertexUV, vertexuV});
        }
    }

    return Mesh{vertices, triangles, {}};
}

/**
 * Generate unit plane with a definition of width * height (which are uint16_t so the maximum amount
 * of vertices is 2^32)
 * @param width how many vertices on the x axis
 * @param height how many vertices on the z axis
 * @return Corresponding Mesh
 */
Mesh Mesh::Plane(uint16_t width, uint16_t height) {
    vector<Vertex> vertices((width+1) * (height+1));
    vector<uint32_t> triangles;
    uint32_t index = 0;

    for (uint16_t y = 0; y <= height; y++) {
        for (uint16_t x = 0; x <= width; x++) {
            vec3 position = {2.0f * x / (float)width - 1.0f, 0, 2.0f * y / (float)height - 1.0f};
            vec3 normal = {0.0f, 1.0f, 0.0f};
            vec2 textCoord = {(float)x / (float)width, (float)y / (float)height};

            vertices[index++] = {position, normal, textCoord};

            if (x > 0 && y > 0) {
                triangles.insert(triangles.end(),
                                 {(uint32_t)(y)*(width+1) + x, (uint32_t)(y - 1) * (width+1) + (x - 1),
                                  (uint32_t)(y)*(width+1) + (x - 1)});

                triangles.insert(triangles.end(),
                                 {(uint32_t)(y)*(width+1) + x, (uint32_t)(y - 1) * (width+1) + (x - 1),
                                  (uint32_t)(y - 1) * (width+1) + x});
            }
        }
    }

    return Mesh{vertices, triangles, {}};
}

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma) {
#if DEBUG
    std::cout << "Core::Rendering::Model::TextureFromFile : Creating texture from " << path
              << " file." << std::endl;
#endif
    stbi_set_flip_vertically_on_load(true);

    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
