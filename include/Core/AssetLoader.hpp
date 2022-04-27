//
// Created by Sam on 4/27/2022.
//

#ifndef ELYS_ASSET_LOADER_HPP
#define ELYS_ASSET_LOADER_HPP

#include <filesystem>
#include <vector>
#include <string>
#include <unordered_map>

#include <Render/Texture.hpp>

using std::string;
using std::vector;
using std::filesystem::path;

namespace Elys {
    class AssetLoader {
      public:
        static const path gAssetPath;
        static const vector<string> gImagesExtensions;
      public:
        static void Init();

        static Texture& TextureFromPath(path const &path);
      private:
        static std::unordered_map<string, Texture> gLoadedTextures;
    };
}

#endif // ELYS_ASSET_LOADER_HPP
