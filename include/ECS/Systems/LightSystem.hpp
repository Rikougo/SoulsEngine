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

namespace Elys {
    class LightSystem : public System {
      public:
        static const uint8_t MAX_LIGHT = 5;
      private:
        Mesh LIGHT_MESH;
        std::shared_ptr<Scene> mCurrentScene;
        std::shared_ptr<TrackBallCamera> mCamera;
        std::shared_ptr<Shader> mShader;
        std::shared_ptr<Framebuffer> mFramebuffer;
      public:
        LightSystem(std::shared_ptr<Scene> &scene, std::shared_ptr<TrackBallCamera> &camera, std::shared_ptr<Shader> &shader, std::shared_ptr<Framebuffer> &framebuffer);

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
