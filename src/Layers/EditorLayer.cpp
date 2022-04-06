//
// Created by Sam on 3/11/2022.
//

#include "Layers/EditorLayer.hpp"

#include <filesystem>

#include <imgui.h>

#include <Application.hpp>

#include <GUI/ProfileDisplay.hpp>

namespace Elys {
    void EditorLayer::OnAttach() {
        mDebugName = "Editor";

        mCurrentScene = std::make_shared<Scene>();
        mShader = std::make_shared<Shader>("./shaders/model_vertex.glsl", "./shaders/model_fragment.glsl");
        // TODO Change TrackBallCamera by general Camera object and control it with an instance of CameraController object
        mCamera = std::make_shared<TrackBallCamera>();
        mCamera->SetViewSize(1280, 720);
        mFramebuffer = std::make_shared<Framebuffer>(FramebufferData{
            .Width = 1280,
            .Height = 720
        });

        mRenderSystem = mCurrentScene->RegisterSystem<RenderSystem>(mCurrentScene, mCamera, mShader, mFramebuffer);
        mCurrentScene->SetSystemSignature<RenderSystem, MeshRenderer, Node>();

        auto mesh = Mesh::Sphere();

        /*auto sky = mCurrentScene->CreateEntity("Sky");
        sky.GetComponent<Node>().SetScale({100.0f, 100.0f, 100.0f});
        sky.AddComponent<MeshRenderer>({
            .mesh = mesh,
            .material = Material::FromTexture(std::filesystem::path("./assets/textures/stars_milky_way.jpg")).SetSelfLight(true)
        });

        auto sun = mCurrentScene->CreateEntity("Sun");
        sun.GetComponent<Node>().SetPosition({-2.0f, 0.0f, 0.0f});
        sun.AddComponent<MeshRenderer>({
            .mesh = mesh,
            .material = Material::FromTexture(std::filesystem::path("./assets/textures/8k_sun.jpg")).SetSelfLight(true)
        });

        auto earth = mCurrentScene->CreateEntity("Earth");
        earth.GetComponent<Node>().SetScale({0.35f, 0.35f, 0.35f});
        earth.AddComponent<MeshRenderer>({
            .mesh = mesh,
            .material = Material::FromTexture(std::filesystem::path("./assets/textures/8k_earth_daymap.jpg"))
        });*/

        for(int i = 0; i <= 5; i++) {
            auto line = mCurrentScene->CreateEntity("Line[" + std::to_string(i) + "]");
            for(int j = 0; j <= 5; j++) {
                auto entity = mCurrentScene->CreateEntity("Sphere[" + std::to_string(j) + "]");
                line.GetComponent<Node>().AddChild(&entity.GetComponent<Node>());
                entity.GetComponent<Node>().SetPosition({2.0f * i - 6.0f, 2.0f * j - 6.0f, 0.0f});
                entity.AddComponent<MeshRenderer>({
                    .mesh = mesh,
                    .material = {.metallic = j * 0.2f, .roughness = i * 0.2f, .albedo = {1.0, 0.0, 0.0, 1.0}}
                });
            }

            ELYS_CORE_INFO("Child of line {0}:", i);
            for(auto const child : line.Children()) {
                ELYS_CORE_INFO("\t- {0}", child.GetComponent<Tag>().name);
            }
        }

    }

    void EditorLayer::OnDetach() {
        mCurrentScene.reset();
    }

    void EditorLayer::OnUpdate(float deltaTime) {
        if (mFramebuffer->GetData().Width != mViewport.size.x || mFramebuffer->GetData().Height != mViewport.size.y) {
            mFramebuffer->Resize(mViewport.size.x, mViewport.size.y);
            mCamera->SetViewSize(mViewport.size.x, mViewport.size.y);
        }

        if (mViewportHovered) mRenderSystem->AcceptEvents();
        mRenderSystem->Update(deltaTime);
    }

    void EditorLayer::OnImGuiRender() {
        static bool dockspaceOpen = true;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
            window_flags |= ImGuiWindowFlags_NoBackground;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        if (ImGui::Begin("MainDock", &dockspaceOpen, window_flags)) {
            ImGui::PopStyleVar(3);
            // DockSpace
            ImGuiIO& io = ImGui::GetIO();
            ImGuiStyle& style = ImGui::GetStyle();
            float minWinSizeX = style.WindowMinSize.x;
            style.WindowMinSize.x = 370.0f;
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            }

            style.WindowMinSize.x = minWinSizeX;

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Exit")) {}

                    ImGui::EndMenu();
                }
            } ImGui::EndMenuBar();

            if (ImGui::Begin("Stats")) {
                static bool VSYNC = true;
                static bool LIGHTNING = true;
                bool tempVSYNC = VSYNC;
                bool tempLightning = LIGHTNING;
                ImGui::Checkbox("Vsync", &tempVSYNC);
                if (tempVSYNC != VSYNC) {
                    VSYNC = tempVSYNC;
                    Application::Get().GetWindow().EnableVSync(VSYNC);
                }

                ImGui::Checkbox("Lightning", &tempLightning);
                if (tempLightning != LIGHTNING) {
                    LIGHTNING = tempLightning;
                    mRenderSystem->SetLightning(LIGHTNING);
                }

                ImGui::Text("Framerate : %0.1f", Profile::Framerate);
                ImGui::Text("Total time : %0.3f", Profile::DeltaTime);
                ImGui::Text("DrawnMesh : %d", Profile::DrawnMesh);
            } ImGui::End();

            mGraphScene.OnImGUIRender(mCurrentScene);
            mComponentsEditor.OnImGUIRender(mGraphScene.GetSelected());

            ImGui::ShowDemoWindow();

            // VIEWPORT
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
            if (ImGui::Begin("Viewport")) {
                auto viewportSize = ImGui::GetContentRegionAvail();

                mViewport.size = {viewportSize.x, viewportSize.y};

                // BIND FRAMEBUFFER COLOR DATA TO WINDOW
                unsigned int textureID = mRenderSystem->GetFramebuffer()->GetColorTextureID();
                ImGui::Image(reinterpret_cast<void*>(textureID), viewportSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

                mViewportHovered = ImGui::IsWindowHovered();
                Application::Get().GetImGUILayer().SetBlocking(!mViewportHovered);


            } ImGui::End();
            ImGui::PopStyleVar();

            ImGui::End();
        } else {
            ImGui::PopStyleVar(3);
        }
    }

    void EditorLayer::OnEvent(Event &event) {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(mRenderSystem->OnKeyPressed));
        dispatcher.Dispatch<MouseScrolledEvent>([this](MouseScrolledEvent &event){
            mCamera->Zoom(event.GetYOffset());

            return false;
        });
    }

    void EditorLayer::CreateScene() { mCurrentScene = std::make_shared<Scene>(); }

    void EditorLayer::LoadScene(const std::filesystem::path &path) {}
    void EditorLayer::SaveScene(const std::filesystem::path &path) {}

    bool EditorLayer::OnKeyPressed(KeyPressedEvent &event) {
        if (event.GetKeyCode() == Key::Q && Input::IsKeyPressed(Key::LeftControl)) {
            Application::Get().Shutdown();
        }

        if (event.GetKeyCode() == Key::R && Input::IsKeyPressed(Key::LeftControl)) {
            mShader->Reload("./shaders/model_vertex.glsl", "./shaders/model_fragment.glsl");
        }

        return false;
    }
} // namespace Elys