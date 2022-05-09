//
// Created by sakeiru on 5/3/2022.
//

#ifndef ELYS_PHYSIC_SYSTEM_HPP
#define ELYS_PHYSIC_SYSTEM_HPP

#include <memory>

#include <Core/Logger.hpp>
#include <Core/Base.hpp>

#include <ECS/System.hpp>
#include <ECS/Scene.hpp>

namespace Elys {
    class PhysicSystem : public System {
      public:
        static constexpr glm::vec3 GRAVITY{0.0f, -9.81f, 0.0f};
      private:
        std::vector<AABB*> mConstraints{};
        std::shared_ptr<Scene> mCurrentScene;
        bool mPhysicUpdate = false;
      public:
        explicit PhysicSystem(std::shared_ptr<Scene> &sceneRef) : mCurrentScene(sceneRef) {}

        void SetPhysicUpdate(bool value) {mPhysicUpdate = value;}

        void Update(float deltaTime) override;

        void AddEntity(EntityID newEntity) override;
        void RemoveEntity(EntityID removedEntity) override;
    };
}

#endif // ELYS_PHYSIC_SYSTEM_HPP
