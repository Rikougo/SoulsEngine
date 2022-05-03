//
// Created by Sam on 4/7/2022.
//

#ifndef ELYS_LIGHT_SYSTEM_HPP
#define ELYS_LIGHT_SYSTEM_HPP

#include <memory>

#include <ECS/System.hpp>
#include <ECS/Scene.hpp>

#include <Render/TrackBallCamera.hpp>
#include <Render/Shader.hpp>
#include <Render/Framebuffer.hpp>
#include <Render/Mesh.hpp>

using std::shared_ptr;
using std::unique_ptr;

namespace Elys {
    class LightSystem : public System {
      public:
        static const uint8_t MAX_LIGHT = 5;
      private:
        Mesh LIGHT_MESH;
        shared_ptr<Scene> mCurrentScene;
        shared_ptr<TrackBallCamera> mCamera;
        shared_ptr<Shader> mShader;
        shared_ptr<Framebuffer> mFramebuffer;
      public:
        LightSystem(shared_ptr<Scene> &scene, shared_ptr<TrackBallCamera> &camera, shared_ptr<Shader> &shader, shared_ptr<Framebuffer> &framebuffer);

        ~LightSystem() = default;

        /**
         * @brief has a look on all lights components, will draw it using simple light shader and a
         * sphere mesh (shared for all lights)
         * @param deltaTime time since last update
         */
        void Update(float deltaTime) override;
    };
}

#endif // ELYS_LIGHT_SYSTEM_HPP
