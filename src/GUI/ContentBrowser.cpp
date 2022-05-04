//
// Created by Sam on 4/26/2022.
//

#include <GUI/ContentBrowser.hpp>

namespace Elys::GUI {
    void ContentBrowser::OnImGUIRender(bool *open) {
        if(ImGui::Begin("Content Browser", open)) {
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

                if (path.has_extension() && AssetLoader::gExtensionToDragType.contains(path.extension().string())) {
                    if (ImGui::BeginDragDropSource()) {
                        #ifdef WIN32
                        const wchar_t* itemPath = relativePath.c_str();
                        #else
                        const char* itemPath = relativePath.c_str();
                        #endif
                        const char* dragData = AssetLoader::gExtensionToDragType.at(path.extension().string());
                        #ifdef WIN32
                        ImGui::SetDragDropPayload(dragData, itemPath, ((relativePath.string().size()) + 1) * sizeof(wchar_t));
                        #else
                        ImGui::SetDragDropPayload(dragData, itemPath, ((relativePath.string().size()) + 1) * sizeof(char));
                        #endif

                        ImGui::Text("%ls", itemPath);
                        ImGui::EndDragDropSource();
                    }
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
        }
        ImGui::End();
    }
} // namespace Elys::GUI