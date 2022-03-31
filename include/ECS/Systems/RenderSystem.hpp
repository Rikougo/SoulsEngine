//
// Created by Sam on 3/30/2022.
//

#ifndef ELYS_RENDER_SYSTEM_HPP
#define ELYS_RENDER_SYSTEM_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Core/Logger.hpp>
#include <Core/Input.hpp>

#include <Events/KeyEvent.hpp>
#include <Events/MouseEvent.hpp>

#include <ECS/System.hpp>

#include <Render/Shader.hpp>
#include <Render/Camera.hpp>
#include <Render/TrackBallCamera.hpp>

namespace Elys {
    class Scene;

    class RenderSystem : public System {
      private:
        Scene* mCurrentScene = nullptr;

        bool mFrustumCulling = false;
        bool mDebugMode = false;
        bool mWireframe = true;

        std::unique_ptr<Shader> mShader = nullptr;
        std::unique_ptr<TrackBallCamera> mCamera = nullptr;
      public:
        RenderSystem();

        ~RenderSystem() {
            mShader.reset();
            mCamera.reset();
        }

        void SetCamera(const TrackBallCamera& camera);
        void SetScene(Scene* scene);
        void Update(float deltaTime) override;

        [[nodiscard]] const Camera& MainCamera() const {
            if (!mCamera)
                ELYS_CORE_ERROR("RenderSystem::MainCamera : No camera set on render system.");
            return *mCamera;
        }

        [[nodiscard]] Camera& MainCamera() {
            if (!mCamera)
                ELYS_CORE_ERROR("RenderSystem::MainCamera : No camera set on render system.");
            return *mCamera;
        }

        void OnViewportChange(float width, float height);
        void OnKeyPressed(KeyPressedEvent &event) override;
        void OnMouseScroll(MouseScrolledEvent &event) override;
      private:
        void ProcessInput();
    };
}

#endif // ELYS_RENDER_SYSTEM_HPP
