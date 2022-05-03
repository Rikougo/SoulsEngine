//
// Created by sakeiru on 5/3/2022.
//

#ifndef ELYS_PHYSIC_SYSTEM_HPP
#define ELYS_PHYSIC_SYSTEM_HPP

#include <memory>

#include <Core/Logger.hpp>

#include <ECS/System.hpp>
#include <ECS/Scene.hpp>

namespace Elys {
    class PhysicSystem : public System {
      private:
        std::shared_ptr<Scene> mCurrentScene;
      public:
        PhysicSystem(std::shared_ptr<Scene> &sceneRef) : mCurrentScene(sceneRef) {}

        void Update(float deltaTime) override;
    };
}

#endif // ELYS_PHYSIC_SYSTEM_HPP
