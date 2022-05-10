//
// Created by Sam on 3/11/2022.
//

#include "Layers/EditorLayer.hpp"

#include <filesystem>

#include <imgui.h>

#include <Application.hpp>

#include <utility>

namespace Elys {
    void EditorLayer::OnAttach() {
        mDebugName = "Editor";

        mCurrentScene = std::make_shared<Scene>();
        mShader = std::make_shared<Shader>("./shaders/model_vertex.glsl",
                                           "./shaders/model_fragment.glsl");
        mEditorCamera = std::make_shared<TrackBallCamera>();
        mEditorCamera->SetViewSize(1280, 720);

        mPlayerCamera = std::make_shared<TrackBallCamera>();
        mPlayerCamera->SetViewSize(1280, 720);
        mPlayerCamera->Rotate(0.0f, -0.5f);

        mFramebuffer = std::make_shared<Framebuffer>(
            FramebufferData{.Width = 1280,
                            .Height = 720,
                            .Attachments = {{GL_RGB}, {GL_RED_INTEGER}},
                            .DepthAttachmentFormat = GL_DEPTH24_STENCIL8});

        InitSystems();

        auto ground = mCurrentScene->CreateEntity("Ground");
        ground.GetComponent<Node>().SetPosition(0.0f, -2.0f, 0.0f);
        ground.GetComponent<Node>().SetScale(50);
        ground.AddComponent<MeshRenderer>(
            {.mesh = AssetLoader::MeshesMap()["Plane32x32"],
             .material =
                 Material::FromTexture(
                     AssetLoader::TextureFromPath("textures/wood_wall/Wood_Wall_003_basecolor.jpg"))
                     .SetNormalMap(AssetLoader::TextureFromPath(
                         "textures/wood_wall/Wood_Wall_003_normal.jpg"))
                     .SetTiling({10, 10})});
        ground.AddComponent<RigidBody>(RigidBody{ground.GetComponent<Node>().InheritedPosition(),
                                                 {50.0f, 1.0f, 50.0f},
                                                 glm::mat3{1.0f}});
        ground.GetComponent<RigidBody>().SetIsKinematic(true);

        /*auto obb = mCurrentScene->CreateEntity("Obb");
        obb.GetComponent<Node>().SetPosition(-1.0f, 0.0f, 0.0f);
        obb.AddComponent<MeshRenderer>({
            .mesh = AssetLoader::MeshFromPath("Sphere")
        });
        obb.AddComponent<RigidBody>(RigidBody{
            obb.GetComponent<Node>().InheritedPosition(),
            obb.GetComponent<Node>().InheritedScale(),
            glm::mat3{1.0f}
        });*/

        auto player = mCurrentScene->CreateEntity("Player");
        player.GetComponent<Node>().SetPosition(-1.0f, 5.0f, 0.0f);
        player.AddComponent<MeshRenderer>({
            .mesh = AssetLoader::MeshFromPath("Sphere")
        });
        player.AddComponent<RigidBody>(RigidBody{
            player.GetComponent<Node>().InheritedPosition(),
            player.GetComponent<Node>().InheritedScale(),
            glm::mat3{1.0f}
        });
        player.AddComponent<Player>({});
        player.GetComponent<RigidBody>().SetUseGravity(true);

        /*auto center = mCurrentScene->CreateEntity("Center");
        auto light = mCurrentScene->CreateEntity("Light");
        light.SetParent(center);
        light.GetComponent<Node>().SetPosition(0.0f, 5.0f, 0.0f);
        light.GetComponent<Node>().SetScale(0.1f);
        light.AddComponent<Light>({.color = {1.0f, 1.0f, 1.0f}, .intensity = 200.0f});
        light.AddComponent<MeshRenderer>({
            .mesh = AssetLoader::MeshesMap()["Sphere"],
            .material = Material{.albedo = {1.0f, 1.0f, 1.0f, 1.0f}}.SetSelfLight(true)
        });*/
    }

    void EditorLayer::OnDetach() {
        mLightSystem.reset();
        mRenderSystem.reset();
        mPhysicSystem.reset();
        mPlayerCamera.reset();

        mFramebuffer.reset();
        mEditorCamera.reset();
        mPlayerCamera.reset();
        mShader.reset();
        mCurrentScene.reset();
    }

    void EditorLayer::OnUpdate(float deltaTime) {
        // mCurrentScene->ProcessDestroyQueue();

        // Framebuffer keep track of viewport size since color attachment is used by viewport for
        // the display. Camera should always be the same size of the Framebuffer.
        if ((mFramebuffer->GetData().Width != mViewport.size.x ||
             mFramebuffer->GetData().Height != mViewport.size.y) &&
            (mViewport.size.x != 0 && mViewport.size.y != 0)) {
            mFramebuffer->Resize(mViewport.size.x, mViewport.size.y);
            mEditorCamera->SetViewSize(mViewport.size.x, mViewport.size.y);
            mPlayerCamera->SetViewSize(mViewport.size.x, mViewport.size.y);
        }

        // ORDER HERE IS IMPORTANT
        if (mCurrentState == EditorState::PLAYING) {
            mPlayerSystem->Update(deltaTime);
        }
        mPhysicSystem->Update(deltaTime);
        mLightSystem->Update(deltaTime);
        mRenderSystem->Update(deltaTime);

        if (mViewportHovered && mCurrentState == EditorState::EDITING) {
            auto [mx, my] = ImGui::GetMousePos();
            mx -= mViewport.offset.x;
            my -= mViewport.offset.y;

            auto entityID = mFramebuffer->GetPixel((int)mx, (int)(mViewport.size.y - my), 1);
            mCurrentScene->SetHovered(entityID);

            if (Input::IsMouseButtonPressed(Mouse::ButtonLeft)) {
                auto mPos = Input::GetMousePosition();
                mEditorCamera->MouseInput(mPos.x, mPos.y, Mouse::ButtonLeft);
            } else if (Input::IsMouseButtonPressed(Mouse::ButtonRight)) {
                auto mPos = Input::GetMousePosition();
                mEditorCamera->MouseInput(mPos.x, mPos.y, Mouse::ButtonRight);
            } else {
                mEditorCamera->EndCapture();
            }


            if (!Input::IsKeyPressed(Key::LeftControl)) {
                glm::vec3 cameraDirection{(Input::IsKeyPressed(Key::A) ? -1.0f : (Input::IsKeyPressed(Key::D) ? 1.0f : 0.0f)),
                                          (Input::IsKeyPressed(Key::Q) ? -1.0f : (Input::IsKeyPressed(Key::E) ? 1.0f : 0.0f)),
                                          (Input::IsKeyPressed(Key::S) ? -1.0f : (Input::IsKeyPressed(Key::W) ? 1.0f : 0.0f))};

                mEditorCamera->Pan(cameraDirection * deltaTime);
            }

        }
    }

    void EditorLayer::OnImGuiRender() {
        static bool dockSpaceOpen = true;
        static ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_None;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockSpaceFlags & ImGuiDockNodeFlags_PassthruCentralNode) {
            window_flags |= ImGuiWindowFlags_NoBackground;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        if (ImGui::Begin("MainDock", &dockSpaceOpen, window_flags)) {
            ImGui::PopStyleVar(3);
            // DockSpace
            ImGuiIO &io = ImGui::GetIO();
            ImGuiStyle &style = ImGui::GetStyle();
            float minWinSizeX = style.WindowMinSize.x;
            style.WindowMinSize.x = 370.0f;
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockSpaceFlags);
            }

            style.WindowMinSize.x = minWinSizeX;

            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Exit", "Ctrl + Q"))
                        Application::Get().Shutdown();
                    if (ImGui::MenuItem("Save scene", "Ctrl + S"))
                        AssetLoader::SerializeScene(mCurrentScene, "scene.escene");
                    if (ImGui::MenuItem("Reload shader", "Ctrl + R"))
                        mShader->Reload("./shaders/model_vertex.glsl",
                                        "./shaders/model_fragment.glsl");
                    if (ImGui::MenuItem("Toggle playmode", "Ctrl + P"))
                        TogglePlayMode();

                    ImGui::EndMenu();
                }
            }
            ImGui::EndMenuBar();

            // ImGui::ShowDemoWindow();

            if (ImGui::Begin("Settings")) {
                if (ImGui::CollapsingHeader("Editor camera", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::BeginTable("##EditorCamera", 2, ImGuiTableFlags_NoPadInnerX);

                    ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthFixed, 100.0f); // Default to 100.0f
                    ImGui::TableSetupColumn("widget", ImGuiTableColumnFlags_WidthStretch); // Default to auto

                    ImGui::TableNextColumn();
                    ImGui::Text("Position");
                    ImGui::TableNextColumn();
                    auto position = mEditorCamera->GetPosition();
                    GUI::SliderVec3("##Position", position, 0.1f, false);

                    ImGui::TableNextColumn();
                    ImGui::Text("Speed");
                    ImGui::TableNextColumn();
                    ImGui::DragFloat("##Speed", &mEditorCamera->speed, 0.1f);

                    ImGui::TableNextColumn();
                    ImGui::Text("Sensitivity");
                    ImGui::TableNextColumn();
                    ImGui::DragFloat("##Sensitivity", &mEditorCamera->sensitivity, 0.1f);

                    ImGui::EndTable();
                }

                if (ImGui::CollapsingHeader("Stats", ImGuiTreeNodeFlags_DefaultOpen)) {
                    static bool VSYNC = true;
                    bool tempVSYNC = VSYNC;

                    ImGui::Checkbox("Vsync", &tempVSYNC);
                    if (tempVSYNC != VSYNC) {
                        VSYNC = tempVSYNC;
                        Application::Get().GetWindow().EnableVSync(VSYNC);
                    }

                    ImGui::Text("Framerate : %0.1f", Profile::Framerate);
                    ImGui::Text("Total time : %0.3f", Profile::DeltaTime);
                    ImGui::Text("DrawnMesh : %d", Profile::DrawnMesh);

                    ImGui::Text("%0.3fx%0.3f", Input::GetMousePosition().x,
                                Input::GetMousePosition().y);
                    ImGui::Text("%0.3fx%0.3f", mViewport.offset.x, mViewport.offset.y);
                }
            }
            ImGui::End();

            mGraphScene.OnImGUIRender(mCurrentScene, nullptr);
            mComponentsEditor.OnImGUIRender(
                mCurrentScene->EntityFromID(mCurrentScene->GetSelected()), nullptr);
            mContentBrowser.OnImGUIRender(nullptr);

            // Viewport: where the 3D scene is drawn
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
            if (ImGui::Begin("Viewport")) {
                /*if (ImGui::BeginMenuBar()) {
                    ImGui::Button("Play");
                } ImGui::EndMenuBar();*/

                auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
                auto viewportOffset = ImGui::GetWindowPos();
                auto viewportSize = ImGui::GetContentRegionAvail();

                // it is viewport position in screen space, useful to have relative mouse position
                // in viewport
                mViewport.offset = {viewportMinRegion.x + viewportOffset.x,
                                    viewportMinRegion.y + viewportOffset.y};
                mViewport.size = {viewportSize.x, viewportSize.y};

                // Bind Framebuffer color texture to an ImGui image
                unsigned int textureID = mRenderSystem->GetFramebuffer()->GetColorTextureID(0);
                ImGui::Image((ImTextureID)((size_t)textureID), viewportSize, ImVec2{0, 1},
                             ImVec2{1, 0});

                mViewportHovered = ImGui::IsWindowHovered();

                // If viewport is not hovered, no need to let event goes further than GUI
                Application::Get().GetImGUILayer().SetBlocking(!mViewportHovered);

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload *payload =
                            ImGui::AcceptDragDropPayload(AssetLoader::DragDropType::FILE_SCENE)) {
                        const auto *path = (const wchar_t *)payload->Data;
                        ChangeScene(AssetLoader::SceneFromPath(path));
                    }

                    ImGui::EndDragDropTarget();
                }
            } ImGui::End();

            ImGui::PopStyleVar();
        } else {
            ImGui::PopStyleVar(3);
        }
        ImGui::End();
    }

    void EditorLayer::OnEvent(Event &event) {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));

        if (mCurrentState == EditorState::EDITING) {
            dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(mRenderSystem->OnKeyPressed));
            dispatcher.Dispatch<MouseScrolledEvent>([this](MouseScrolledEvent &event) {
                mEditorCamera->Zoom(event.GetYOffset() *
                                    (Input::IsKeyPressed(Key::LeftShift) ? 10.0f : 0.1f));

                return false;
            });

            dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent &event) {
                if (event.GetMouseButton() != Mouse::ButtonMiddle)
                    return false;

                auto [mx, my] = ImGui::GetMousePos();
                mx -= mViewport.offset.x;
                my -= mViewport.offset.y;

                auto entityID = mFramebuffer->GetPixel((int)mx, (int)(mViewport.size.y - my), 1);

                mCurrentScene->SetSelected(entityID);

                ELYS_CORE_INFO("Clicked entity {0} at [x: {1}, y: {2}]", entityID, mx, my);

                return false;
            });
        }
    }

    void EditorLayer::CreateScene() { mCurrentScene = std::make_shared<Scene>(); }

    void EditorLayer::ChangeScene(std::shared_ptr<Scene> newScene) {
        mCurrentScene = std::move(newScene);

        InitSystems();
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent &event) {
        auto kc = event.GetKeyCode();

        // --- APPLICATION INTERACTION --- //
        if (kc == Key::P && Input::IsKeyPressed(Key::LeftControl)) {
            TogglePlayMode();
        }

        if (kc == Key::A && Input::IsKeyPressed(Key::LeftControl)) {
            Application::Get().Shutdown();
        }

        if (kc == Key::S && Input::IsKeyPressed(Key::LeftControl)) {
            AssetLoader::SerializeScene(mCurrentScene, "scene.escene");
        }

        if (kc == Key::R && Input::IsKeyPressed(Key::LeftControl)) {
            mShader->Reload("./shaders/model_vertex.glsl", "./shaders/model_fragment.glsl");
        }

        if (kc == Key::D && Input::IsKeyPressed(Key::LeftControl)) {
            mRenderSystem->ToggleDebugMode();
        }

        return false;
    }

    void EditorLayer::InitSystems() {
        mLightSystem = mCurrentScene->RegisterSystem<LightSystem>(mCurrentScene, mEditorCamera,
                                                                  mShader, mFramebuffer);
        mCurrentScene->SetSystemSignature<LightSystem, Light, Node>();

        mRenderSystem = mCurrentScene->RegisterSystem<RenderSystem>(mCurrentScene, mEditorCamera,
                                                                    mShader, mFramebuffer);
        mCurrentScene->SetSystemSignature<RenderSystem, MeshRenderer, Node>();

        mPhysicSystem = mCurrentScene->RegisterSystem<PhysicSystem>(mCurrentScene);
        mCurrentScene->SetSystemSignature<PhysicSystem, RigidBody, MeshRenderer, Node>();

        mPlayerSystem = mCurrentScene->RegisterSystem<PlayerSystem>(mCurrentScene, mPlayerCamera);
        mCurrentScene->SetSystemSignature<PlayerSystem, Player, Node>();
    }

    void EditorLayer::TogglePlayMode() {
        mCurrentState =
            (mCurrentState == EditorState::EDITING) ? EditorState::PLAYING : EditorState::EDITING;

        /*switch (mCurrentState) {
        case EditorState::EDITING:
            mRenderSystem->SetCamera(mEditorCamera);
            break;
        case EditorState::PLAYING:
            mRenderSystem->SetCamera(mPlayerCamera);
            break;
        }*/

        mPhysicSystem->SetPhysicUpdate(mCurrentState == EditorState::PLAYING);
    }
} // namespace Elys