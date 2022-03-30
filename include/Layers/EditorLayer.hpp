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
#include <Render/Mesh.hpp>

namespace Elys {
    class EditorLayer : public Layer {
      public:
        EditorLayer() = default;
        void OnAttach() override;
        void OnDetach() override;

        void OnUpdate(float deltaTime) override;
        void OnImGuiRender() override;
        void OnEvent(Event& e) override;

      private:
        void Render();
        void PhysicUpdate(float deltaTime);

        void DrawEntity(Entity const &entity, Frustum const &frustum);

        void ListChildren(Entity const &entity);

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
        Shader* mShader = nullptr;
        Shader* mGridShader = nullptr;

        TrackBallCamera mCamera;

        float mRightPanelWidth = 0.33f;
        Scene mCurrentScene;
        bool mWireframeMode = false;
        bool mDrawBoundingBoxes = false;
        bool mUseFrustumCulling = true;

        Entity mSelected;
        bool mUniformScale = true;

        Mesh sphereMesh;
        Texture sphereTexture;

        std::array<std::pair<float, bool>, 5'000> mFloatSpeed;
        float mCumulator = 0.0f;
    };
}

#endif // ELYS_ECS_LAYER_HPP
