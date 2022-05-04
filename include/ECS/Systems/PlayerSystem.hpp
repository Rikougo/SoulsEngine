//
// Created by sakeiru on 5/3/2022.
//

#ifndef ELYS_PLAYER_SYSTEM_HPP
#define ELYS_PLAYER_SYSTEM_HPP

#include <memory>

#include <glm/glm.hpp>

#include <Core/Input.hpp>

#include <ECS/Scene.hpp>
#include <ECS/System.hpp>

#include <ECS/Components/Node.hpp>
#include <ECS/Components/Player.hpp>
#include <Render/TrackBallCamera.hpp>

namespace Elys {
    class PlayerSystem : public System {
      private:
        std::shared_ptr<Scene> mCurrentScene;
        std::shared_ptr<TrackBallCamera> mPlayerCamera;
      public:
        PlayerSystem(std::shared_ptr<Scene> &sceneRef, std::shared_ptr<TrackBallCamera> &playerCamera) : mCurrentScene(sceneRef), mPlayerCamera(playerCamera) {}

        void Update(float deltaTime) override;
    };
} // namespace Elys

#endif // ELYS_PLAYER_SYSTEM_HPP
