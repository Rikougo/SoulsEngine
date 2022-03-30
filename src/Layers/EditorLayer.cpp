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
        /*auto w = Application::Get().GetWindow();
        glViewport(0, 0, (GLsizei)w.GetWidth() * (1 - mRightPanelWidth), (GLsizei)w.GetHeight());
        mCamera.SetViewSize((float)w.GetWidth() * (1 - mRightPanelWidth), (float)w.GetHeight());*/

        mShader = new Shader("./shaders/model_vertex.glsl", "./shaders/model_fragment.glsl");

        std::string texturePath = "./assets/textures";
        Texture earthTexture = GenerateTexture("8k_earth_daymap.jpg", texturePath);
        Texture earthCloudTexture = GenerateTexture("8k_earth_clouds.jpg", texturePath);
        Texture sunTexture = GenerateTexture("8k_sun.jpg", texturePath);

        sphereMesh = Mesh::Sphere();
        auto sunEntity = mCurrentScene.CreateEntity("Sun");
        sunEntity.AddComponent(sphereMesh);
        sunEntity.AddComponent(sunTexture);

        auto earthEntity = mCurrentScene.CreateEntity("Earth");
        earthEntity.AddComponent(sphereMesh);
        earthEntity.AddComponent(earthTexture);
        earthEntity.GetComponent<Node>().SetPosition(1.5, 0.0, 0.0);

        sunEntity.GetComponent<Node>().AddChild(&earthEntity.GetComponent<Node>());
    }
    void EditorLayer::OnDetach() {}

    void EditorLayer::OnUpdate(float deltaTime) {
        Render();
        PhysicUpdate(deltaTime);
    }

    void EditorLayer::Render() {
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mShader->Use();
        mShader->SetMat4("uProjection", mCamera.GetProjection());
        mShader->SetMat4("uView", mCamera.GetView());

        mShader->SetVec3("uLightPosition", mCamera.GetPosition());

        Profile::DrawnMesh = 0;
        Profile::ComputingBoundingBoxes = 0.0f;
        Profile::DrawingMeshes = 0.0f;

        auto frustum = mCamera.GetFrustum();

        for (auto &entity : mCurrentScene) {
            DrawEntity(entity, frustum);
        }
    }

    void EditorLayer::DrawEntity(Entity const &entity, Frustum const &frustum) {
        if (!entity.HasComponent<Mesh>())
            return;

        auto then = (float)glfwGetTime();
        auto const &node = entity.GetComponent<Node>();
        auto model = node.InheritedTransform();
        auto const &mesh = entity.GetComponent<Mesh>();
        Profile::DrawingMeshes += (float)glfwGetTime() - then;

        then = (float)glfwGetTime();
        auto const &boundingBox = mesh.GetAABB();

        if (mUseFrustumCulling && !boundingBox.IsInFrustum(frustum, model)) {
            Profile::ComputingBoundingBoxes += (float)glfwGetTime() - then;
            return;
        }
        Profile::ComputingBoundingBoxes += (float)glfwGetTime() - then;

        if (mDrawBoundingBoxes) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            mShader->SetVec3("uColor", {0.0, 0.5, 0.0});
            mShader->SetBool("uShadingOn", false);
            // TODO DRAW DEBUG INFORMATION SUCH AS BOUNDING BOXES
        }

        then = (float)glfwGetTime();
        bool hasTexture = entity.HasComponent<Texture>();
        mShader->SetBool("uHasTexture", hasTexture);
        if (hasTexture) {
            auto const &texture = entity.GetComponent<Texture>();
            glActiveTexture(GL_TEXTURE0);
            mShader->SetInt("uTexture", 0);
            glBindTexture(GL_TEXTURE_2D, texture.id);
        } else {
            mShader->SetVec3("uColor", {0.7, 0.5, 1.0});
        }

        mShader->SetVec3("uAmbient", vec3{0.1f, 0.1f, 0.1f});

        mShader->SetMat4("uModel", model);
        mShader->SetBool("uShadingOn", true);

        // glPolygonMode(GL_FRONT_AND_BACK, mWireframeMode ? GL_LINE : GL_FILL);
        glBindVertexArray(mesh.VAO());
        glDrawElements(GL_TRIANGLES, (GLsizei)mesh.IndicesSize(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        Profile::DrawingMeshes += (float)glfwGetTime() - then;

        Profile::DrawnMesh++;
    }

    void EditorLayer::PhysicUpdate(float deltaTime) { }

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

            if (ImGui::BeginChild("Camera", ImVec2(0, 75))) {
                auto p = mCamera.GetPosition();
                auto t = mCamera.GetTarget();
                auto r = mCamera.GetRotation();

                ImGui::Text("Position : Vector3f(%0.1f, %0.1f, %0.1f)", p.x, p.y, p.z);
                ImGui::Text("Rotation: [PHI=%0.1f,THETA=%0.1f]", r.x, r.y);
                ImGui::Text("Target: Vector3f(%0.1f, %0.1f, %0.1f)", t.x, t.y, t.z);
                ImGui::EndChild();
            }

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
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent &event) {
        if (event.GetKeyCode() == GLFW_KEY_Z) mWireframeMode = !mWireframeMode;
        if (event.GetKeyCode() == GLFW_KEY_F) mUseFrustumCulling = !mUseFrustumCulling;
        return false;
    }

    bool EditorLayer::OnKeyReleased(KeyReleasedEvent &event) { return false; }

    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent &event) {
        mCamera.StartCapture(event.GetMouseButton());
        return false;
    }

    bool EditorLayer::OnMouseButtonReleased(MouseButtonReleasedEvent &event) {
        mCamera.EndCapture();
        return false;
    }

    bool EditorLayer::OnMouseMove(MouseMovedEvent &event) {
        mCamera.MouseInput(event.GetX(), event.GetY());
        return false;
    }

    bool EditorLayer::OnMouseScroll(MouseScrolledEvent &event) {
        mCamera.Zoom(event.GetYOffset());
        return false;
    }

    bool EditorLayer::OnWindowResize(WindowResizeEvent &event) {
        glViewport(0, 0, (GLsizei)event.GetWidth() * (1 - mRightPanelWidth), (GLsizei)event.GetHeight());
        mCamera.SetViewSize((float)event.GetWidth() * (1 - mRightPanelWidth), (float)event.GetHeight());

        return false;
    }

    void EditorLayer::CreateScene() { mCurrentScene = Scene(); }

    void EditorLayer::LoadScene(const std::filesystem::path &path) {}
    void EditorLayer::SaveScene(const std::filesystem::path &path) {}
} // namespace Elys