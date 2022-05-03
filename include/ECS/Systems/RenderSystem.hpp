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

using std::shared_ptr;

namespace Elys {
    class RenderSystem : public System {
      private:
        shared_ptr<Scene> mCurrentScene;

        bool mProcessInputs = false;
        bool mFrustumCulling = false;
        bool mDebugMode = false;
        bool mWireframe = false;
        bool mLightning = true;

        shared_ptr<TrackBallCamera> mCamera;
        shared_ptr<Shader> mShader;
        shared_ptr<Shader> mOutlineShader;
        shared_ptr<Shader> mLineShader;

        shared_ptr<Framebuffer> mFramebuffer;
      public:
        RenderSystem(shared_ptr<Scene> &scene, shared_ptr<TrackBallCamera> &camera, shared_ptr<Shader> &shader, shared_ptr<Framebuffer> &framebuffer);

        ~RenderSystem() = default;

        void SetCamera(const TrackBallCamera& camera);
        void SetScene(shared_ptr<Scene> &sceneRef);

        void AcceptEvents() { mProcessInputs = true;}
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
        bool OnMouseScroll(MouseScrolledEvent &event);

        std::shared_ptr<Framebuffer> GetFramebuffer() { return mFramebuffer; }
      private:
        void ProcessInput();
    };
}

#endif // ELYS_RENDER_SYSTEM_HPP
