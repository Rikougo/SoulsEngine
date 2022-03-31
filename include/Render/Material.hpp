//
// Created by Sam on 3/30/2022.
//

#ifndef ELYS_MATERIAL_HPP
#define ELYS_MATERIAL_HPP

#include <string>
#include <filesystem>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <Core/Logger.hpp>

using glm::vec3;
using glm::vec4;

namespace Elys {
    unsigned int GLTextureFromFile(const std::filesystem::path &path);

    struct Texture {
        unsigned int id;
        std::string path;

        static Texture FromPath(const std::filesystem::path &path) {
            return Texture {
                .id = GLTextureFromFile(path),
                .path = path.string()
            };
        }

        static Texture* PtrFromPath(const std::filesystem::path &path) {
            return new Texture {
                .id = GLTextureFromFile(path),
                .path = path.string()
            };
        }
    };

    struct Material {
        vec3 ambient{0.0f, 0.0f, 0.0f};
        vec3 diffuse{1.0f, 1.0f, 1.0f};
        vec3 specular{0.0f, 0.0f, 0.0f};

        Texture *texture = nullptr;
        vec4 color{0.58f, 0.58f, 0.58f, 1.0f};
        Texture *normalMap = nullptr;

        static Material FromTexture(const Texture &texture) {
            return {
                .texture = new Texture(texture),
                .color = {0.0f, 0.0f, 0.0f, 0.0f}
            };
        }

        static Material FromTexture(const std::filesystem::path &texturePath) {
            return FromTexture(Texture::FromPath(texturePath));
        }
    };
}

#endif // ELYS_MATERIAL_HPP
