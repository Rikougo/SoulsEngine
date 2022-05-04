//
// Created by Sam on 3/11/2022.
//

#ifndef ELYS_ECS_LAYER_HPP
#define ELYS_ECS_LAYER_HPP

#include <filesystem>
#include <string>

#include <glm/glm.hpp>

#include <Core/Layer.hpp>
#include <Core/AssetLoader.hpp>

#include <ECS/Scene.hpp>
#include <ECS/Systems/LightSystem.hpp>
#include <ECS/Systems/RenderSystem.hpp>
#include <ECS/Systems/PhysicSystem.hpp>
#include <ECS/Systems/PlayerSystem.hpp>

#include <Core/Event.hpp>

#include <Render/Shader.hpp>
#include <Render/TrackBallCamera.hpp>

#include <GUI/GraphScene.hpp>
#include <GUI/ComponentsEditor.hpp>
#include <GUI/ContentBrowser.hpp>

using std::shared_ptr;

namespace Elys {
    enum class EditorState {
        EDITING,
        PLAYING
    };

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
        void ChangeScene(shared_ptr<Scene> newScene);
      public:
        struct Viewport {
            glm::vec2 offset{0, 0}, size{0, 0};
        };
      private:
        EditorState mCurrentState{EditorState::EDITING};
        Viewport mViewport;
        bool mViewportHovered = false;
        shared_ptr<Scene> mCurrentScene;

        // --- SYSTEMS --- //
        shared_ptr<LightSystem> mLightSystem;
        shared_ptr<RenderSystem> mRenderSystem;
        shared_ptr<PhysicSystem> mPhysicSystem;
        shared_ptr<PlayerSystem> mPlayerSystem;

        shared_ptr<Framebuffer> mDepthbuffer;
        shared_ptr<Framebuffer> mFramebuffer;
        shared_ptr<TrackBallCamera> mEditorCamera;
        shared_ptr<TrackBallCamera> mPlayerCamera;
        shared_ptr<Shader> mShader;

        GUI::ContentBrowser mContentBrowser;
        GUI::GraphScene mGraphScene;
        GUI::ComponentsEditor mComponentsEditor;
    };
}

#endif // ELYS_ECS_LAYER_HPP
