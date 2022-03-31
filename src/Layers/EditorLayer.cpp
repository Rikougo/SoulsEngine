//
// Created by Sam on 3/11/2022.
//

#include "Layers/EditorLayer.hpp"

#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>

#include <Application.hpp>

#include <GUI/GUI.hpp>
#include <GUI/TextInput.hpp>
#include <GUI/ProfileDisplay.hpp>

namespace Elys {
    void EditorLayer::OnAttach() {
        std::filesystem::path texturePath("./assets/textures/");
        auto mesh = Mesh::Sphere();
        auto material = Material::FromTexture(std::filesystem::path("./assets/textures/earth.jpg"));

        material.normalMap = Texture::PtrFromPath(std::filesystem::path("./assets/textures/normal_map/8k_earth_normal_map.png"));

        auto meshRenderer = MeshRenderer{
            .Mesh = mesh,
            .Material = material
        };

        auto earth = mCurrentScene.CreateEntity("Earth");
        earth.AddComponent<MeshRenderer>(meshRenderer);
    }

    void EditorLayer::OnDetach() {}

    void EditorLayer::OnUpdate(float deltaTime) {
        mCurrentScene.OnUpdate(deltaTime);
    }

    void EditorLayer::OnImGuiRender() {
        GUI::ProfileDisplay(GUI::TOP_LEFT);

        static int corner = 1;
        ImGuiIO &io = ImGui::GetIO();
        ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse;
        if (corner != -1) {
            const float PAD = 0.0f;
            const ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImVec2 work_pos =
                viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = viewport->WorkSize;
            ImVec2 window_pos, window_pos_pivot;
            window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
            window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
            window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
            window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
            window_flags |= ImGuiWindowFlags_NoMove;
        }
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * mRightPanelWidth, io.DisplaySize.y));
        if(ImGui::Begin("Entities", nullptr, window_flags)) {
            Application::Get().GetImGUILayer().SetBlocking(
                ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows));

            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("Entity")) {
                    if (ImGui::MenuItem("New")) {
                        auto newEntity = mCurrentScene.CreateEntity();

                        if (mSelected.IsValid())
                            mSelected.GetComponent<Node>().AddChild(&newEntity.GetComponent<Node>());
                    }
                    if (ImGui::MenuItem("Delete", "del", false, mSelected.IsValid())) {
                        mCurrentScene.DestroyEntity(mSelected);
                        mSelected = Entity();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            // left
            {
                if(ImGui::BeginChild("selector", ImVec2(150, 0), true)) {
                    for (const Entity &entity : mCurrentScene) {
                        auto const &node = entity.GetComponent<Node>();

                        if (node.Parent())
                            continue;

                        auto const &tag = entity.GetComponent<Tag>();

                        std::stringstream treeID, label;
                        treeID << "##" << tag.name << "#" << entity.ID();
                        label << tag.name << "#" << entity.ID();

                        if (ImGui::Selectable(label.str().c_str(), mSelected == entity, 0, ImVec2(75, 0)))
                            mSelected = entity;
                        if (!node.Children().empty()) {
                            ImGui::SameLine();
                            if (ImGui::TreeNode(treeID.str().c_str())) {
                                ListChildren(entity);
                                ImGui::TreePop();
                            }
                        }
                    }
                    ImGui::EndChild();
                }
            }
            ImGui::SameLine();

            // Right
            if (mSelected.IsValid()) {
                ImGui::BeginGroup();
                if (ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()))) // Leave room for 1 line below us
                {
                    auto &name = mSelected.GetComponent<Tag>().name;

                    GUI::InputText("##NAME#Selected", &name, 0, nullptr, nullptr);
                    ImGui::Separator();

                    auto &node = mSelected.GetComponent<Node>();

                    auto pos = node.LocalPosition();
                    auto rot = eulerAngles(node.LocalRotation()) * (180.0f / static_cast<float>(M_PI));
                    auto scale = node.LocalScale();

                    ImGui::Text("Position : ");
                    ImGui::SameLine();
                    ImGui::DragFloat3("##POSITION#Selected", &pos[0], 0.05f, -100.f, 100.f, "%0.2f");
                    ImGui::Text("Rotation : ");
                    ImGui::SameLine();
                    ImGui::DragFloat3("##ROTATION#Selected", &rot[0], 1.0f, -360, 360, "%0.2f");
                    ImGui::Text("Scale : ");
                    ImGui::SameLine();
                    ImGui::DragFloat3("##SCALE#Selected", &scale[0], 0.1f, 0.1f, 10.f, "%0.2f");
                    ImGui::SameLine();
                    ImGui::Checkbox("##SCALE#Selected#uniform", &mUniformScale);

                    if (mUniformScale) {
                        for (uint8_t i = 0; i < 3; i++) {
                            if (scale[i] != node.LocalScale()[i]) {
                                node.SetScale(scale[i]);
                                break;
                            }
                        }
                    } else node.SetScale(scale);

                    node.SetPosition(pos);
                    node.SetRotation(glm::quat(rot * (static_cast<float>(M_PI) / 180.0f)));


                    ImGui::EndChild();
                }
                ImGui::EndGroup();
            }

            ImGui::End();
        }
    }

    void EditorLayer::ListChildren(Entity const &entity) {
        for (auto child : entity.Children()) {
            auto const &childTag = child.GetComponent<Tag>();

            std::stringstream childTreeID, childLabel;
            childTreeID << "##" << childTag.name << "#" << child.ID();
            childLabel << childTag.name << "#" << child.ID();

            if (ImGui::Selectable(childLabel.str().c_str(), mSelected == child, 0, ImVec2(75, 0)))
                mSelected = child;
            if (child.Children().size() > 0) {
                ImGui::SameLine();
                if (ImGui::TreeNode(childTreeID.str().c_str())) {
                    ListChildren(child);
                    ImGui::TreePop();
                }
            }
        }
    }

    void EditorLayer::OnEvent(Event &event) {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
        dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButtonReleased));
        dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseMove));
        dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(EditorLayer::OnMouseScroll));
        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyReleased));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(EditorLayer::OnWindowResize));

        mCurrentScene.OnEvent(event);
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent &event) { return false; }
    bool EditorLayer::OnKeyReleased(KeyReleasedEvent &event) { return false; }
    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent &event) { return false; }
    bool EditorLayer::OnMouseButtonReleased(MouseButtonReleasedEvent &event) { return false; }
    bool EditorLayer::OnMouseMove(MouseMovedEvent &event) { return false; }
    bool EditorLayer::OnMouseScroll(MouseScrolledEvent &event) { return false; }
    bool EditorLayer::OnWindowResize(WindowResizeEvent &event) { return false; }

    void EditorLayer::CreateScene() { mCurrentScene = Scene(); }

    void EditorLayer::LoadScene(const std::filesystem::path &path) {}
    void EditorLayer::SaveScene(const std::filesystem::path &path) {}
} // namespace Elys