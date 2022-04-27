//
// Created by Sam on 4/26/2022.
//

#include <GUI/ContentBrowser.hpp>

namespace Elys::GUI {
    void ContentBrowser::OnImGUIRender() {
        ImGui::Begin("Content Browser");

        if (mCurrentPath != std::filesystem::path(AssetLoader::gAssetPath)) {
            if (ImGui::Button("<-")) {
                mCurrentPath = mCurrentPath.parent_path();
            }
        }

        static float padding = 32.0f;
        static float thumbnailSize = 64.0f;
        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1)
            columnCount = 1;

        ImGui::Columns(columnCount, nullptr, false);

        for (auto &directoryEntry : std::filesystem::directory_iterator(mCurrentPath)) {
            const auto &path = directoryEntry.path();
            auto relativePath = std::filesystem::relative(path, AssetLoader::gAssetPath);
            std::string filenameString = relativePath.filename().string();

            ImGui::PushID(filenameString.c_str());
            Texture icon = directoryEntry.is_directory() ? mFolderIcon : mFileIcon;
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::ImageButton((ImTextureID)(size_t)icon.ID(), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});

            if (ImGui::BeginDragDropSource()) {
                const wchar_t *itemPath = relativePath.c_str();
                ImGui::SetDragDropPayload("CONTENT_BROWSER_IMAGE", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
                ImGui::Text("%ls", itemPath);
                ImGui::EndDragDropSource();
            }

            ImGui::PopStyleColor();
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                if (directoryEntry.is_directory())
                    mCurrentPath /= path.filename();
            }
            ImGui::TextWrapped("%s", filenameString.c_str());

            ImGui::NextColumn();

            ImGui::PopID();
        }

        ImGui::Columns(1);

        // TODO: status bar
        ImGui::End();
    }
} // namespace Elys::GUI