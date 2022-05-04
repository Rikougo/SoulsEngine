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

namespace Elys {
    enum class EditorState {
        EDITING,
        PLAYING
    };

    class EditorLayer : public Layer {
      public:
        // --- LIFE CYCLE METHODS --- //
        EditorLayer() = default;
        ~EditorLayer() override = default;
        void OnAttach() override;
        void OnDetach() override;

        void OnUpdate(float deltaTime) override;
        void OnImGuiRender() override;
        void OnEvent(Event& e) override;
      private:
        bool OnKeyPressed(KeyPressedEvent &event);

        void CreateScene();
        void ChangeScene(std::shared_ptr<Scene> newScene);

        void InitSystems();

        void TogglePlayMode();
      public:
        struct Viewport {
            glm::vec2 offset{0, 0}, size{0, 0};
        };
      private:
        EditorState mCurrentState{EditorState::EDITING};
        Viewport mViewport;
        bool mViewportHovered = false;
        std::shared_ptr<Scene> mCurrentScene;

        // --- SYSTEMS --- //
        std::shared_ptr<LightSystem> mLightSystem;
        std::shared_ptr<RenderSystem> mRenderSystem;
        std::shared_ptr<PhysicSystem> mPhysicSystem;
        std::shared_ptr<PlayerSystem> mPlayerSystem;

        // --- DISPLAY STUFF --- //
        std::shared_ptr<Framebuffer> mFramebuffer;
        std::shared_ptr<TrackBallCamera> mEditorCamera;
        std::shared_ptr<TrackBallCamera> mPlayerCamera;
        std::shared_ptr<Shader> mShader;

        GUI::ContentBrowser mContentBrowser{};
        GUI::GraphScene mGraphScene{};
        GUI::ComponentsEditor mComponentsEditor{};
    };
}

#endif // ELYS_ECS_LAYER_HPP
