//
// Created by Sam on 3/30/2022.
//

#ifndef ELYS_RENDER_SYSTEM_HPP
#define ELYS_RENDER_SYSTEM_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Core/Logger.hpp>
#include <Core/Input.hpp>

#include <Core/Event.hpp>

#include <ECS/System.hpp>
#include <ECS/Scene.hpp>

#include <Render/Shader.hpp>
#include <Render/Framebuffer.hpp>
#include <Render/Camera.hpp>
#include <Render/TrackBallCamera.hpp>

namespace Elys {
    class RenderSystem : public System {
      private:
        std::shared_ptr<Scene> mCurrentScene;

        bool mFrustumCulling = false;
        bool mDebugMode = true;
        bool mWireframe = false;
        bool mLightning = true;

        std::shared_ptr<TrackBallCamera> mCamera;
        std::shared_ptr<Shader> mShader;
        std::shared_ptr<Shader> mOutlineShader;
        std::shared_ptr<Shader> mLineShader;

        std::shared_ptr<Framebuffer> mFramebuffer;
      public:
        RenderSystem(std::shared_ptr<Scene> &scene, std::shared_ptr<TrackBallCamera> &camera, std::shared_ptr<Shader> &shader, std::shared_ptr<Framebuffer> &framebuffer);

        ~RenderSystem() = default;

        void SetCamera(std::shared_ptr<TrackBallCamera>& camera);
        void SetScene(std::shared_ptr<Scene> &sceneRef);
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

        void SetViewportSize(glm::vec2 offset, glm::vec2 size);
        bool OnKeyPressed(KeyPressedEvent &event);
        // bool OnMouseScroll(MouseScrolledEvent &event);

        std::shared_ptr<Framebuffer> GetFramebuffer() { return mFramebuffer; }

        void ToggleDebugMode();
        void SetDebugMode(bool value);
        [[nodiscard]] bool IsDebugMode() const { return mDebugMode; };
    };
}

#endif // ELYS_RENDER_SYSTEM_HPP
