//
// Created by Sam on 4/27/2022.
//

#include <Core/AssetLoader.hpp>

namespace Elys {
    const path AssetLoader::gAssetPath = "./assets";
    std::unordered_map<string, Texture> AssetLoader::gLoadedTextures = std::unordered_map<string, Texture>();

    void AssetLoader::Init() {}

    Texture &AssetLoader::TextureFromPath(const path &path) {
        if (!gLoadedTextures.contains(path.string())) {
            gLoadedTextures[path.string()] = Texture::FromPath(gAssetPath / path);
        }

        return gLoadedTextures[path.string()];
    }
}