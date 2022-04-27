//
// Created by Sam on 4/27/2022.
//

#ifndef ELYS_TEXTURE_HPP
#define ELYS_TEXTURE_HPP

#include <string>
#include <filesystem>

#include <glm/glm.hpp>

using glm::vec2;

namespace Elys {
    class Texture {
      private:
        unsigned int mId;
        int mHeight, mWidth;
        std::filesystem::path mPath;
        bool mValid = false;

      public:
        Texture() : mHeight(-1), mWidth(-1), mId(0), mPath("") {}

        Texture(unsigned int id, std::filesystem::path path, int width, int height)
            : mId(id), mPath(std::move(path)), mWidth(width), mHeight(height), mValid(true) {}

        Texture(Texture& texture) = default;
        Texture(const Texture& texture) = default;

        /*Texture(Texture&&) = default;
        Texture& operator=(Texture&&) = default;*/

        bool Valid() const { return mValid; }
        [[nodiscard]] unsigned int ID() const { return mId; }
        std::filesystem::path GetPath() { return mPath; }
        [[nodiscard]] std::string GetName() { return mPath.filename().string(); }
        [[nodiscard]] vec2 GetSize() const { return {mWidth, mHeight}; }

        static Texture FromPath(const std::filesystem::path &path);
    };
}

#endif // ELYS_TEXTURE_HPP
