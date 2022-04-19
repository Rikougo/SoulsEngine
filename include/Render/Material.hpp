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

using glm::vec2;
using glm::vec3;
using glm::vec4;

namespace Elys {
    // Texture GLTextureFromFile(const std::filesystem::path &path);

    class Texture {
      private:
        unsigned int mId;
        int mHeight, mWidth;
        std::filesystem::path mPath;
        bool mValid = false;
      public:
        Texture() {}

        Texture(unsigned int id, std::filesystem::path path, int width, int height) :
            mId(id), mPath(std::move(path)), mWidth(width), mHeight(height), mValid(true) {}

        bool Valid() { return mValid; }
        unsigned int ID() const { return mId; }
        std::filesystem::path GetPath() { return mPath; }
        vec2 GetSize() const { return {mWidth, mHeight}; }

        static Texture FromPath(const std::filesystem::path &path);
    };

    struct Material {
        float metallic = 0.0f;
        float roughness = 0.0f;
        bool selfLight = false;

        vec4 albedo{0.58f, 0.58f, 0.58f, 1.0f};
        std::optional<Texture> texture;
        std::optional<Texture> normalMap;

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

        /** @brief
         *  Create Material texture, it does have default Ambient/Diffuse/Specular specs but have white opaque color.
         */
        static Material FromTexture(const std::filesystem::path &texturePath, vec4 a = {1.0f, 1.0f, 1.0f, 1.0f}, float roughness = 0.0f, float metallic = 0.0f) {
            return FromTexture(Texture::FromPath(texturePath), a, roughness, metallic);
        }

        Material& SetSelfLight(bool value) {
            selfLight = value;
            return *this;
        }

        Material& SetNormalMap(const Texture &value) {
            normalMap = value;
            return *this;
        }

        Material& ResetNormalMap() {
            normalMap.reset();
            return *this;
        }

        Material& SetTexture(const Texture &value) {
            texture = value;
            return *this;
        }

        Material& ResetTexture() {
            texture.reset();
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
    };
}

#endif // ELYS_MATERIAL_HPP
