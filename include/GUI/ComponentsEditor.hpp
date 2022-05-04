//
// Created by sakeiru on 4/1/2022.
//

#ifndef ELYS_COMPONENTS_EDITOR_HPP
#define ELYS_COMPONENTS_EDITOR_HPP

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <GUI/GUI.hpp>
#include <GUI/ContentBrowser.hpp>

#include <ECS/Scene.hpp>

namespace Elys::GUI {
    class ComponentsEditor {
      public:
        void OnImGUIRender(Entity entity, bool* open) {
            if (ImGui::Begin("Components Editor", open, ImGuiWindowFlags_AlwaysAutoResize)) {
                if (!entity.IsValid()) {
                    ImGui::End();
                    return;
                }

                auto &node = entity.GetComponent<Node>();

                ImGui::PushID(node.name.c_str());
                {

                    ImGui::BeginTable("Informations", 3);

                    ImGui::TableSetupColumn("enabled", ImGuiTableColumnFlags_WidthFixed,
                                            15.0f); // Default to 100.0f
                    ImGui::TableSetupColumn("name",
                                            ImGuiTableColumnFlags_WidthStretch); // Default to auto
                    ImGui::TableSetupColumn("add", ImGuiTableColumnFlags_WidthFixed,
                                            15.0f); // Default to auto

                    ImGui::TableNextColumn();
                    bool enabled = node.LocalEnabled();
                    ImGui::Checkbox("##enabled", &enabled);
                    if (node.LocalEnabled() ^ enabled)
                        node.SetEnabled(enabled);

                    ImGui::TableNextColumn();
                    std::vector<char> cTagName(node.name.c_str(), node.name.c_str() + node.name.size() + 1);
                    if (ImGui::InputText("##Tag", cTagName.data(), cTagName.size())) {
                        node.name = std::string(cTagName.begin(), cTagName.end());
                    }

                    ImGui::TableNextColumn();
                    if (ImGui::Button("+")) {
                        ImGui::OpenPopup("AddComponentPopup");
                    }

                    if (ImGui::BeginPopupContextItem("AddComponentPopup"))
                    {
                        if (ImGui::Selectable("MeshRenderer", false, entity.HasComponent<MeshRenderer>() ? ImGuiSelectableFlags_Disabled : 0)) {
                            entity.AddComponent<MeshRenderer>({
                                .mesh = AssetLoader::MeshesMap()["Cube"]
                            });
                        }

                        if (ImGui::Selectable("Light", false, entity.HasComponent<Light>() ? ImGuiSelectableFlags_Disabled : 0)) {
                            entity.AddComponent<Light>({});
                        }

                        if (ImGui::Selectable("RigidBody", false, entity.HasComponent<RigidBody>() ? ImGuiSelectableFlags_Disabled : 0)) {
                            entity.AddComponent<RigidBody>({});
                        }

                        if (ImGui::Selectable("Player", false, entity.HasComponent<Player>() ? ImGuiSelectableFlags_Disabled : 0)) {
                            entity.AddComponent<Player>({});
                        }
                        ImGui::EndPopup();
                    }

                    ImGui::EndTable();

                    ImGui::SameLine();

                    // --- NODE COMPONENT ---
                    // Shared by every entity
                    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                        NodeEditor("##Transform", node);
                    }

                    // --- MESH RENDERER ---
                    if (entity.HasComponent<MeshRenderer>()) {
                        if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
                            MeshRenderEditor("##MeshRenderer", entity.GetComponent<MeshRenderer>());
                        }
                    }

                    if (entity.HasComponent<Light>()) {
                        if (ImGui::CollapsingHeader("Light Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
                            LightEditor("##Light", entity.GetComponent<Light>());
                        }
                    }

                    if (entity.HasComponent<RigidBody>()) {
                        if (ImGui::CollapsingHeader("RigidBody", ImGuiTreeNodeFlags_DefaultOpen)) {
                            RigidBodyEditor("##RigidBody", entity.GetComponent<RigidBody>());
                        }
                    }

                    if (entity.HasComponent<Player>()) {
                        if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen)) {
                            PlayerEditor("##Player", entity.GetComponent<Player>());
                        }
                    }
                }
                ImGui::PopID();
            }
            ImGui::End();
        };

        void NodeEditor(std::string const &label, Node &node) {
            auto pos = node.LocalPosition();
            auto rot = eulerAngles(node.LocalRotation()) * (180.0f / static_cast<float>(M_PI));
            auto scale = node.LocalScale();

            static bool uniformScale = true;

            auto tableFlags = ImGuiTableFlags_NoPadInnerX;
            ImGui::BeginTable(label.c_str(), 2, tableFlags);

            ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthFixed,
                                    100.0f); // Default to 100.0f
            ImGui::TableSetupColumn("widget",
                                    ImGuiTableColumnFlags_WidthStretch); // Default to auto

            ImGui::TableNextColumn();
            ImGui::Text("Position");
            ImGui::TableNextColumn();
            SliderVec3("##Scale", pos);
            ImGui::TableNextColumn();
            ImGui::Text("Rotation");
            ImGui::TableNextColumn();
            SliderVec3("##Scale", rot);
            ImGui::TableNextColumn();
            ImGui::Text("Scale");
            ImGui::TableNextColumn();
            SliderVec3("##Scale", scale);

            ImGui::EndTable();

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
        }

        void MeshRenderEditor(std::string const &label, MeshRenderer &meshRenderer) {
            auto tableFlags = ImGuiTableFlags_NoPadInnerX;
            ImGui::BeginTable(label.c_str(), 3, tableFlags);

            ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthFixed,
                                    100.0f); // Default to 100.0f
            ImGui::TableSetupColumn("widget",
                                    ImGuiTableColumnFlags_WidthStretch); // Default to auto
            ImGui::TableSetupColumn("widget_modifier", ImGuiTableColumnFlags_WidthFixed,
                                    20.0f); // Default to auto

            ImGui::TableNextColumn();
            ImGui::Text("Mesh");
            ImGui::TableNextColumn();
            if (ImGui::BeginCombo("##MeshSelector", meshRenderer.mesh.Path().c_str(), 0))
            {
                auto meshes = AssetLoader::MeshesMap();
                for (auto pair : meshes)
                {
                    const bool is_selected = (pair.first == meshRenderer.mesh.Path());
                    if (ImGui::Selectable(pair.first.c_str(), is_selected)) {
                        meshRenderer.mesh = pair.second;
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            // ImGui::Combo
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("Color");
            ImGui::TableNextColumn();
            auto &a = meshRenderer.material.albedo;
            if (ImGui::ColorButton("MaterialAlbedo", ImVec4(a.r, a.g, a.b, a.a), 0,
                                   ImVec2(ImGui::GetColumnWidth(), ImGui::GetFrameHeight())))
                ImGui::OpenPopup("MaterialAlbedo");
            if (ImGui::BeginPopup("MaterialAlbedo")) {
                ImGui::ColorPicker4("##MaterialAlbedoPicker", glm::value_ptr(a),
                                    ImGuiColorEditFlags_None, nullptr);
                ImGui::EndPopup();
            }
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("UV Tiling");
            ImGui::TableNextColumn();
            GUI::SliderVec2("##UVTiling", meshRenderer.material.tiling);
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("Texture");
            ImGui::TableNextColumn();
            GUI::TextureInput(meshRenderer.material.texture, "Texture");
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("Normal map");
            ImGui::TableNextColumn();
            GUI::TextureInput(meshRenderer.material.normalMap, "Normalmap");
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("Height map");
            ImGui::TableNextColumn();
            GUI::TextureInput(meshRenderer.material.heightMap, "Heightmap");
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("Shaded");
            ImGui::TableNextColumn();
            ImGui::Checkbox("##Shaded", &meshRenderer.material.shaded);
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("Metallic");
            ImGui::TableNextColumn();
            ImGui::DragFloat("##Metallic", &meshRenderer.material.metallic, 0.05f, 0.0f, 1.0f);
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("Roughness");
            ImGui::TableNextColumn();
            ImGui::DragFloat("##Roughness", &meshRenderer.material.roughness, 0.05f, 0.0f, 1.0f);
            ImGui::TableNextRow();

            ImGui::EndTable();
        }

        void LightEditor(std::string const &label, Light &light) {
            auto tableFlags = ImGuiTableFlags_NoPadInnerX;
            ImGui::BeginTable(label.c_str(), 2, tableFlags);

            ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthFixed,
                                    100.0f); // Default to 100.0f
            ImGui::TableSetupColumn("widget",
                                    ImGuiTableColumnFlags_WidthStretch); // Default to auto

            ImGui::TableNextColumn();
            ImGui::Text("Color");
            ImGui::TableNextColumn();
            auto &lc = light.color;
            if (ImGui::ColorButton("LightColor", ImVec4(lc.r, lc.g, lc.b, 1.0), 0,
                                   ImVec2(ImGui::GetColumnWidth(), ImGui::GetFrameHeight())))
                ImGui::OpenPopup("LightColor");
            if (ImGui::BeginPopup("LightColor")) {
                ImGui::ColorPicker3("##LightColorPicker", glm::value_ptr(lc),
                                    ImGuiColorEditFlags_None);
                ImGui::EndPopup();
            }

            ImGui::TableNextColumn();
            ImGui::Text("Intensity");
            ImGui::TableNextColumn();
            ImGui::DragFloat("##intensity", &light.intensity, 1.0f, 0.0f, 500.0f, "%0.1f");

            ImGui::EndTable();
        }

        void RigidBodyEditor(std::string const &label, RigidBody &rBody) {
            auto tableFlags = ImGuiTableFlags_NoPadInnerX;
            ImGui::BeginTable(label.c_str(), 2, tableFlags);

            ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthFixed, 100.0f); // Default to 100.0f
            ImGui::TableSetupColumn("widget", ImGuiTableColumnFlags_WidthStretch); // Default to auto

            ImGui::TableNextColumn();
            ImGui::Text("Collided");
            ImGui::TableNextColumn();
            auto v = rBody.Velocity();
            ImGui::Text("[%0.2f,%0.2f]", v.x, v.y);

            ImGui::EndTable();
        }

        void PlayerEditor(std::string const &label, Player &player) {
            auto tableFlags = ImGuiTableFlags_NoPadInnerX;
            ImGui::BeginTable(label.c_str(), 2, tableFlags);

            ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthFixed, 100.0f); // Default to 100.0f
            ImGui::TableSetupColumn("widget", ImGuiTableColumnFlags_WidthStretch); // Default to auto

            ImGui::TableNextColumn();
            ImGui::Text("Speed");
            ImGui::TableNextColumn();
            ImGui::DragFloat("##speed", &player.speed, 0.1f, 0.0f, 20.0f, "%0.1f");

            ImGui::EndTable();
        }
    };
}

#endif // ELYS_COMPONENTS_EDITOR_HPP
