//
// Created by Sam on 4/21/2022.
//

#include <GUI/GUI.hpp>

namespace Elys::GUI {
    void ConfigureWindowPos(Corner corner, ImVec2 &windowPos, ImVec2 &windowPosPivot,
                            float padding) {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        // Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_pos = viewport->WorkPos;
        ImVec2 work_size = viewport->WorkSize;
        windowPos.x = (corner & 1) ? (work_pos.x + work_size.x - padding) : (work_pos.x + padding);
        windowPos.y = (corner & 2) ? (work_pos.y + work_size.y - padding) : (work_pos.y + padding);
        windowPosPivot.x = (corner & 1) ? 1.0f : 0.0f;
        windowPosPivot.y = (corner & 2) ? 1.0f : 0.0f;
    }

    void SliderVec2(std::string const &label, glm::vec2 &data, float speed, bool input) {
        ImGuiIO &io = ImGui::GetIO();
        ImGui::PushID(label.c_str());
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.15, 0.15, 0.15, 1.0));
        auto tableFlags = ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_SizingFixedSame;

        ImGui::BeginTable(label.c_str(), 2, tableFlags);

        ImGui::TableSetupColumn("x", ImGuiTableColumnFlags_WidthStretch); // Default to auto
        ImGui::TableSetupColumn("y", ImGuiTableColumnFlags_WidthStretch); // Default to auto

        auto sliderFlag = input ? 0 : ImGuiSliderFlags_NoInput;

        // X editor
        ImGui::TableNextColumn();
        ImGui::PushItemWidth(ImGui::GetColumnWidth() * 0.8f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(1.0f, 0.25f, 0.25f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));
        ImGui::DragFloat("##X", &data[0], speed, 0.0f, 0.0f, "%0.2f", sliderFlag);
        ImGui::PopStyleColor(3);

        // Y editor
        ImGui::TableNextColumn();
        ImGui::PushItemWidth(ImGui::GetColumnWidth() * 0.8f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.5f, 1.0f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.25f, 1.0f, 0.25f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.1f, 1.0f, 0.1f, 1.0f));
        ImGui::DragFloat("##Y", &data[1], speed, 0.0f, 0.0f, "%0.2f", sliderFlag);
        ImGui::PopStyleColor(3);

        ImGui::EndTable();

        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        ImGui::PopID();
    }

    void SliderVec3(std::string const &label, glm::vec3 &data, float speed, bool input) {
        ImGuiIO &io = ImGui::GetIO();
        ImGui::PushID(label.c_str());
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.15, 0.15, 0.15, 1.0));
        auto tableFlags = ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_SizingFixedSame;

        ImGui::BeginTable(label.c_str(), 3, tableFlags);

        ImGui::TableSetupColumn("x", ImGuiTableColumnFlags_WidthStretch); // Default to auto
        ImGui::TableSetupColumn("y", ImGuiTableColumnFlags_WidthStretch); // Default to auto
        ImGui::TableSetupColumn("z", ImGuiTableColumnFlags_WidthStretch); // Default to auto

        auto sliderFlag = input ? 0 : ImGuiSliderFlags_NoInput;

        // X editor
        ImGui::TableNextColumn();
        ImGui::PushItemWidth(ImGui::GetColumnWidth() * 0.8f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(1.0f, 0.25f, 0.25f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));
        ImGui::DragFloat("##X", &data[0], speed, 0.0f, 0.0f, "%0.2f", sliderFlag);
        ImGui::PopStyleColor(3);

        // Y editor
        ImGui::TableNextColumn();
        ImGui::PushItemWidth(ImGui::GetColumnWidth() * 0.8f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.5f, 1.0f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.25f, 1.0f, 0.25f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.1f, 1.0f, 0.1f, 1.0f));
        ImGui::DragFloat("##Y", &data[1], speed, 0.0f, 0.0f, "%0.2f", sliderFlag);
        ImGui::PopStyleColor(3);

        // Z editor
        ImGui::TableNextColumn();
        ImGui::PushItemWidth(ImGui::GetColumnWidth() * 0.8f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.5f, 0.5f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.25f, 0.25f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.1f, 0.1f, 1.0f, 1.0f));
        ImGui::DragFloat("##Z", &data[2], speed, 0.0f, 0.0f, "%0.2f", sliderFlag);
        ImGui::PopStyleColor(3);
        ImGui::EndTable();

        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        ImGui::PopID();
    }

    void TextureInput(std::optional<Texture> &texture, char const* ID) {
        ImGui::PushID(ID);
        std::string texPath = texture ? texture->GetName() : "None";
        ImGui::Selectable(texPath.c_str());

        if (ImGui::BeginPopupContextItem()) {
            bool disabled = !texture;
            if (disabled) {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }

            if (ImGui::Button("Reset texture")) {
                texture.reset();
            }

            if (disabled) {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }

            ImGui::EndPopup();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(AssetLoader::DragDropType::FILE_IMAGE)) {
                const auto *path = (const wchar_t *)payload->Data;
                texture = AssetLoader::TextureFromPath(path);
            }

            ImGui::EndDragDropTarget();
        }
        ImGui::PopID();
    }
} // namespace Elys::GUI