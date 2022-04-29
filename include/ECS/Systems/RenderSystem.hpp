//
// Created by Sam on 3/30/2022.
//

#ifndef ELYS_RENDER_SYSTEM_HPP
#define ELYS_RENDER_SYSTEM_HPP

#include <Core/Logger.hpp>
#include <Core/Input.hpp>

#include "Core/Event.hpp"

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
        bool mProcessInputs = false;
        bool mFrustumCulling = false;
        bool mDebugMode = false;
        bool mWireframe = false;
        bool mLightning = true;

        shared_ptr<Scene> mCurrentScene;
        shared_ptr<TrackBallCamera> mCamera;
        shared_ptr<Shader> mShader;
        shared_ptr<Shader> mOutlineShader;
        shared_ptr<Framebuffer> mFramebuffer;
      public:
        RenderSystem(shared_ptr<Scene> &scene, shared_ptr<TrackBallCamera> &camera, shared_ptr<Shader> &shader, shared_ptr<Framebuffer> &framebuffer);

        ~RenderSystem() = default;

        void SetCamera(const TrackBallCamera& camera);
        void SetScene(shared_ptr<Scene> &sceneRef);

        /**
         * @brief Important to call before Update() if you want render system to take input in account
         * (may be deleted later, for a better design option)
         */
        void AcceptEvents() { mProcessInputs = true;}
        /**
         * @brief has a look on all mesh renderer components, will draw it (using node component for
         * position and camera model/projection). It also makes use of material (contained in mesh renderer)
         * to draw Color/Texture/NormalMap(WIP).
         * @param deltaTime time since last time
         */
        void Update(float deltaTime) override;

        [[nodiscard]] const Camera& MainCamera() const {
            if (!mCamera)
                throw std::runtime_error("RenderSystem::MainCamera : No camera set on render system.");
            return *mCamera;
        }

        [[nodiscard]] Camera& MainCamera() {
            if (!mCamera)
                throw std::runtime_error("RenderSystem::MainCamera : No camera set on render system.");
            return *mCamera;
        }

        bool OnKeyPressed(KeyPressedEvent &event);

        void SetLightning(bool enabled) { mLightning = enabled; }
        [[nodiscard]] bool UseLightning() { return mLightning; }

        shared_ptr<Framebuffer> GetFramebuffer() { return mFramebuffer; }
      private:
        void ProcessInput();
    };
}

#endif // ELYS_RENDER_SYSTEM_HPP
