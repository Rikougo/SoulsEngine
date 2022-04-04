//
// Created by sakeiru on 4/1/2022.
//

#ifndef ELYS_COMPONENTS_EDITOR_HPP
#define ELYS_COMPONENTS_EDITOR_HPP

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <ECS/Scene.hpp>

namespace Elys::GUI {
    class ComponentsEditor {
      public:
        void OnImGUIRender(Entity &entity) {
            static bool componentsEditorOpen = true;
            if (ImGui::Begin("Components Editor", &componentsEditorOpen, ImGuiWindowFlags_AlwaysAutoResize)) {
                if (!entity.IsValid()) {
                    ImGui::End();
                    return;
                }

                auto& tag = entity.GetComponent<Tag>().name;

                ImGui::PushID(tag.c_str());

                char buffer[256];
                memset(buffer, 0, sizeof(buffer));
                strncpy(buffer, tag.c_str(), sizeof(buffer));
                if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
                {
                    tag = std::string(buffer);
                }

                // --- NODE COMPONENT ---
                // Shared by every entity
                if (ImGui::TreeNode("Transform")) {
                    auto &node = entity.GetComponent<Node>();

                    auto pos = node.LocalPosition();
                    auto rot =
                        eulerAngles(node.LocalRotation()) * (180.0f / static_cast<float>(M_PI));
                    auto scale = node.LocalScale();

                    static bool uniformScale = true;

                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
                    Vec3Editor("Position", pos);
                    Vec3Editor("Rotation", rot, 1.0f);
                    Vec3Editor("Scale", scale);

                    if (uniformScale) {
                        for (uint8_t i = 0; i < 3; i++) {
                            if (scale[i] != node.LocalScale()[i]) {
                                node.SetScale(scale[i]);
                                break;
                            }
                        }
                    } else
                        node.SetScale(scale);

                    node.SetPosition(pos);
                    node.SetRotation(glm::quat(rot * (static_cast<float>(M_PI) / 180.0f)));

                    ImGui::PopStyleVar();
                    ImGui::TreePop();
                }

                // --- MESH RENDERER ---
                if (entity.HasComponent<MeshRenderer>()) {
                    if (ImGui::TreeNode("mesh Renderer")) {
                        auto &meshRenderer = entity.GetComponent<MeshRenderer>();

                        ImGui::ColorEdit4("Color", glm::value_ptr(meshRenderer.material.color), ImGuiColorEditFlags_NoInputs);

                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                        Vec3Editor("Ambient", meshRenderer.material.ambient);
                        Vec3Editor("Diffuse", meshRenderer.material.diffuse);
                        Vec3Editor("Specular", meshRenderer.material.specular);
                        ImGui::PopStyleVar();
                        ImGui::TreePop();
                    }
                }

                ImGui::PopID();
            } ImGui::End();
        };

        void Vec3Editor(std::string const &label, glm::vec3 &data, float speed = 0.1f) {
            ImGui::PushID(label.c_str());

            auto tableFlags = ImGuiTableFlags_NoPadInnerX;
            ImGui::BeginTable(label.c_str(), 4, tableFlags);

            ImGui::TableSetupColumn("one", ImGuiTableColumnFlags_WidthFixed, 100.0f); // Default to 100.0f
            ImGui::TableSetupColumn("two", ImGuiTableColumnFlags_WidthStretch);         // Default to auto
            ImGui::TableSetupColumn("three", ImGuiTableColumnFlags_WidthStretch);       // Default to auto
            ImGui::TableSetupColumn("four", ImGuiTableColumnFlags_WidthStretch);        // Default to auto

            ImGui::TableNextColumn();
            ImGui::Text("%s", label.c_str());
            ImGui::TableNextColumn();
            ImGui::DragFloat("##X", &data[0], speed, 0.0f, 0.0f, "%0.2f");
            ImGui::TableNextColumn();
            ImGui::DragFloat("##Y", &data[1], speed, 0.0f, 0.0f, "%0.2f");
            ImGui::TableNextColumn();
            ImGui::DragFloat("##Z", &data[2], speed, 0.0f, 0.0f, "%0.2f");

            ImGui::EndTable();

            ImGui::PopID();
        }
    };
}

#endif // ELYS_COMPONENTS_EDITOR_HPP
