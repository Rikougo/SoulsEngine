//
// Created by Sam on 3/11/2022.
//

#include "Layers/EditorLayer.hpp"

#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
        auto material = Material::FromTexture(std::filesystem::path("./assets/textures/earth.jpg"));

        auto meshRenderer = MeshRenderer{
            .Mesh = mesh,
            .Material = material
        };

        auto sky = mCurrentScene->CreateEntity("Sky");
        sky.AddComponent<MeshRenderer>({
            .Mesh = mesh,
            .Material = Material::FromTexture(std::filesystem::path("./assets/textures/stars_milky_way.jpg"))
        });

        auto sun = mCurrentScene->CreateEntity("Sun");
        sun.AddComponent<MeshRenderer>({
            .Mesh = mesh,
            .Material = Material::FromTexture(std::filesystem::path("./assets/textures/8k_sun.jpg"))
        });

        auto earth = mCurrentScene->CreateEntity("Earth");
        earth.AddComponent<MeshRenderer>({
            .Mesh = mesh,
            .Material = Material::FromTexture(std::filesystem::path("./assets/textures/8k_earth_daymap.jpg"))
        });

    }

    void EditorLayer::OnDetach() {
        mCurrentScene.reset();
    }

    void EditorLayer::OnUpdate(float deltaTime) {
        if (mFramebuffer->GetData().Width != mViewport.size.x || mFramebuffer->GetData().Height != mViewport.size.y) {
            mFramebuffer->Resize(mViewport.size.x, mViewport.size.y);
            mCamera->SetViewSize(mViewport.size.x, mViewport.size.y);
        }

        mCurrentScene->OnUpdate(deltaTime);
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
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
            ELYS_CORE_INFO("No background");
            window_flags |= ImGuiWindowFlags_NoBackground;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);

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
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);1
                if (ImGui::MenuItem("New", "Ctrl+N"));
                if (ImGui::MenuItem("Open...", "Ctrl+O"));
                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"));
                if (ImGui::MenuItem("Exit"));
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        if (ImGui::Begin("Stats")) {
            ImGui::Text("Framerate : %0.1f", Profile::Framerate);
            ImGui::Text("Total time : %0.3f", Profile::DeltaTime);
            ImGui::Text("Draw time : %0.3f", Profile::DrawingTime);
            ImGui::Text("\t- Computing bounding boxes : %0.3f", Profile::ComputingBoundingBoxes);
            ImGui::Text("\t- Drawing meshes : %0.3f", Profile::DrawingMeshes);
            ImGui::Text("GUI time : %0.3f", Profile::GUITime);
            ImGui::Text("DrawnMesh : %d", Profile::DrawnMesh);
            ImGui::End();
        }

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

            ImGui::End();
            ImGui::PopStyleVar();
        }

        ImGui::End();
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

    bool EditorLayer::OnKeyPressed(KeyPressedEvent &event) { return false; }
    bool EditorLayer::OnKeyReleased(KeyReleasedEvent &event) { return false; }
    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent &event) { return false; }
    bool EditorLayer::OnMouseButtonReleased(MouseButtonReleasedEvent &event) { return false; }
    bool EditorLayer::OnMouseMove(MouseMovedEvent &event) { return false; }
    bool EditorLayer::OnMouseScroll(MouseScrolledEvent &event) { return false; }
    bool EditorLayer::OnWindowResize(WindowResizeEvent &event) { return false; }

    void EditorLayer::CreateScene() { mCurrentScene = std::make_shared<Scene>(); }

    void EditorLayer::LoadScene(const std::filesystem::path &path) {}
    void EditorLayer::SaveScene(const std::filesystem::path &path) {}
} // namespace Elys