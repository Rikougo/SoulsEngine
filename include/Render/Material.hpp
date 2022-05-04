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

#include <Render/Texture.hpp>

using glm::vec2;
using glm::vec3;
using glm::vec4;

namespace Elys {
    struct Material {
        float metallic = 0.0f;
        float roughness = 0.0f;
        bool shaded = true;

        vec4 albedo{0.58f, 0.58f, 0.58f, 1.0f};
        std::optional<Texture> texture;
        std::optional<Texture> normalMap;
        std::optional<Texture> heightMap;
        vec2 tiling{1.0f, 1.0f};

        /** @brief
         *  Create Material texture, it does have default Ambient/Diffuse/Specular specs but have white opaque color.
         */
        static Material FromTexture(const Texture &texture, vec4 a = {1.0f, 1.0f, 1.0f, 1.0f}, float roughness = 0.0f, float metallic = 0.0f) {
            return {
                .metallic = metallic,
                .roughness = roughness,
                .albedo = a,
                .texture = texture,
            };
        }

        Material& SetSelfLight(bool value) {
            shaded = value;
            return *this;
        }

        Material& SetNormalMap(const Texture& value) {
            if (!value.Valid()) return *this;
            normalMap = value;
            return *this;
        }

        Material& ResetNormalMap() {
            normalMap.reset();
            return *this;
        }

        Material& SetTexture(const Texture &value) {
            if (!value.Valid()) return *this;
            texture = value;
            return *this;
        }

        Material& ResetTexture() {
            texture.reset();
            return *this;
        }

        Material& SetHeightMap(const Texture &value) {
            if (!value.Valid()) return *this;
            heightMap = value;
            return *this;
        }

        Material& ResetHeightMap() {
            heightMap.reset();
            return *this;
        }

        Material& SetAlbedo(float x, float y, float z, float w = 1.0f) {
            albedo = {x, y, z, w};
            return *this;
        }

        Material& SetAlbedo(const vec4 &value) {
            albedo = value;
            return *this;
        }

        Material& SetMetallic(float value) {
            metallic = value;
            return *this;
        }

        Material& SetRoughness(float value) {
            roughness = value;
            return *this;
        }

        Material& SetTiling(vec2 value) {
            tiling = value;
            return *this;
        }

        [[nodiscard]] std::string TexturePath() const { return texture ? texture->GetPath().string() : "None"; }
        [[nodiscard]] std::string NormalMapPath() const { return normalMap ? normalMap->GetPath().string() : "None"; }
        [[nodiscard]] std::string HeightMapPath() const { return heightMap ? heightMap->GetPath().string() : "None"; }
    };
}

#endif // ELYS_MATERIAL_HPP
