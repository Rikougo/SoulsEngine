//
// Created by Sam on 3/11/2022.
//

#ifndef ELYS_ECS_LAYER_HPP
#define ELYS_ECS_LAYER_HPP

#include <filesystem>

#include <glm/glm.hpp>

#include <Core/Layer.hpp>

#include <ECS/Scene.hpp>
#include <ECS/Components.hpp>

#include <Events/Event.hpp>
#include <Events/KeyEvent.hpp>
#include <Events/MouseEvent.hpp>
#include <Events/ApplicationEvent.hpp>

#include <Render/Shader.hpp>
#include <Render/TrackBallCamera.hpp>
#include <Render/TextureLoader.hpp>
#include <Render/MeshLoader.hpp>

namespace Elys {
    class ECSLayer : public Layer {
      public:
        ECSLayer() = default;
        void OnAttach() override;
        void OnDetach() override;

        void OnUpdate(float deltaTime) override;
        void OnImGuiRender() override;
        void OnEvent(Event& e) override;

      private:
        void Render();
        void PhysicUpdate(float deltaTime);

        void DrawEntity(const Entity &entity, glm::mat4 parent = glm::mat4{1.0f});

        bool OnKeyPressed(KeyPressedEvent &event);
        bool OnKeyReleased(KeyReleasedEvent &event);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& event);
        bool OnMouseButtonReleased(MouseButtonReleasedEvent& event);
        bool OnMouseMove(MouseMovedEvent& event);
        bool OnMouseScroll(MouseScrolledEvent& event);
        bool OnWindowResize(WindowResizeEvent &event);

        void CreateScene();
        void LoadScene(const std::filesystem::path& path);
        void SaveScene(const std::filesystem::path& path);
      private:
        Shader* mShader;

        TrackBallCamera mCamera;

        float mRightPanelWidth = 0.33f;
        Scene mCurrentScene;
        bool mWireframeMode = false;

        Entity mWalking;
        bool mForward = false, mBackward = false, mLeft = false, mRight = false;
        float mTerrainOffset = 0.5f;
        Entity mTerrain;
        Entity mSelected;
    };
}

#endif // ELYS_ECS_LAYER_HPP
