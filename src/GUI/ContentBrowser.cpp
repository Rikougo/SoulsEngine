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

            std::vector<std::filesystem::directory_entry> directories{};
            std::vector<std::filesystem::directory_entry> files{};

            for (auto const &directoryEntry : std::filesystem::directory_iterator(mCurrentPath)) {
                if (directoryEntry.is_directory()) directories.push_back(directoryEntry);
                else files.push_back(directoryEntry);
            }

            std::sort(files.begin(), files.end(), [](std::filesystem::directory_entry &left, std::filesystem::directory_entry &right) {
               if (!left.path().has_extension()) return false;
               if (!right.path().has_extension()) return false;

               return left.path().extension().string() > right.path().extension().string();
            });

            for (auto &directoryEntry : directories) {
                const auto &path = directoryEntry.path();
                std::string filenameString = path.filename().string();

                ImGui::PushID(filenameString.c_str());
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::ImageButton((ImTextureID)(size_t)mFolderIcon.ID(), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});


                ImGui::PopStyleColor();
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    mCurrentPath /= path.filename();
                }
                ImGui::TextWrapped("%s", filenameString.c_str());

                ImGui::NextColumn();

                ImGui::PopID();
            }

            for (auto &directoryEntry : files) {
                const auto &path = directoryEntry.path();
                std::string filenameString = path.filename().string();

                ImGui::PushID(filenameString.c_str());
                Texture icon = (path.has_extension() && path.extension() == ".fbx") ? m3DIcon : mFileIcon;

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::ImageButton((ImTextureID)(size_t)icon.ID(), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});

                if (path.has_extension() && AssetLoader::gExtensionToDragType.contains(path.extension().string())) {
                    if (ImGui::BeginDragDropSource()) {
                        auto relativePath = std::filesystem::relative(path, AssetLoader::gAssetPath);
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
                else if (path.has_extension() && path.extension() == ".fbx") {
                    if (ImGui::BeginPopupContextItem()) {
                        if (ImGui::Selectable("Load mesh")) {
                            auto relativePath = std::filesystem::relative(path, AssetLoader::gAssetPath);
                            AssetLoader::MeshFromPath(relativePath);
                        }
                        ImGui::EndPopup();
                    }
                }

                ImGui::PopStyleColor();

                ImGui::TextWrapped("%s", filenameString.c_str());

                ImGui::NextColumn();

                ImGui::PopID();
            }
            ImGui::Columns(1);
        }
        ImGui::End();
    }
} // namespace Elys::GUI