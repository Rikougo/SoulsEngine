//
// Created by Sam on 3/11/2022.
//

#ifndef ELYS_ECS_LAYER_HPP
#define ELYS_ECS_LAYER_HPP

#include <filesystem>
#include <string>

#include <glm/glm.hpp>

#include <Core/Layer.hpp>

#include <ECS/Scene.hpp>
#include <ECS/Components.hpp>
#include <ECS/Systems/LightSystem.hpp>
#include <ECS/Systems/RenderSystem.hpp>

#include <Events/Event.hpp>
#include <Events/KeyEvent.hpp>
#include <Events/MouseEvent.hpp>
#include <Events/ApplicationEvent.hpp>

#include <Render/Shader.hpp>
#include <Render/TrackBallCamera.hpp>
#include <Render/Mesh.hpp>

#include <GUI/GraphScene.hpp>
#include <GUI/ComponentsEditor.hpp>

using std::shared_ptr;

namespace Elys {
    class EditorLayer : public Layer {
      public:
        EditorLayer() = default;
        ~EditorLayer() = default;
        void OnAttach() override;
        void OnDetach() override;

        void OnUpdate(float deltaTime) override;
        void OnImGuiRender() override;
        void OnEvent(Event& e) override;
      private:
        bool OnKeyPressed(KeyPressedEvent &event);

        void CreateScene();
        void LoadScene(const std::filesystem::path& path);
        void SaveScene(const std::filesystem::path& path);
      public:
        struct Viewport {
            glm::vec2 offset{0, 0}, size{0, 0};
        };
      private:
        Viewport mViewport;
        bool mViewportHovered = false;
        shared_ptr<Scene> mCurrentScene;

        shared_ptr<LightSystem> mLightSystem;
        shared_ptr<RenderSystem> mRenderSystem;
        shared_ptr<Framebuffer> mFramebuffer;
        shared_ptr<TrackBallCamera> mCamera;
        shared_ptr<Shader> mShader;

        GUI::GraphScene mGraphScene;
        GUI::ComponentsEditor mComponentsEditor;
    };
}

#endif // ELYS_ECS_LAYER_HPP
