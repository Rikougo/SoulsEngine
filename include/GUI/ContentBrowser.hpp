//
// Created by Sam on 4/26/2022.
//

#ifndef ELYS_CONTENT_BROWSER_HPP
#define ELYS_CONTENT_BROWSER_HPP

#include <string>
#include <filesystem>

#include <imgui.h>

#include <Core/AssetLoader.hpp>
#include <Render/Material.hpp>

using std::filesystem::path;

namespace Elys::GUI {
    class ContentBrowser {
      public:
        ContentBrowser() : mCurrentPath(AssetLoader::gAssetPath) {
            /*mFileIcon = Texture::FromPath(AssetLoader::gAssetPath / path("icons/file-icon.png"));
            mFolderIcon = Texture::FromPath(AssetLoader::gAssetPath / path("icons/folder-icon.png"));*/
            mFileIcon = AssetLoader::TextureFromPath("icons/file-icon.png");
            mFolderIcon = AssetLoader::TextureFromPath("icons/folder-icon.png");
        }

        void OnImGUIRender();
      private:
        Texture mFileIcon, mFolderIcon;
        path mCurrentPath;
    };
} // namespace Elys::GUI

#endif // ELYS_CONTENT_BROWSER_HPP
