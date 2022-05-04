//
// Created by sakeiru on 5/3/2022.
//

#include <ECS/Systems/PhysicSystem.hpp>

namespace Elys {
    void PhysicSystem::Update(float deltaTime) {
        for(auto id : mEntities) {
            auto entity = mCurrentScene->EntityFromID(id);

            // PHYSICS
            auto& node = entity.GetComponent<Node>();
            auto& rBody = entity.GetComponent<RigidBody>();
            auto& aabb = rBody.GetAABB();
            auto const &mesh = entity.GetComponent<MeshRenderer>().mesh;

            aabb.Update(node.InheritedTransform(), mesh);

            aabb.SetCollided(false);

            for(auto otherID : mEntities) {
                if (otherID == id) continue;

                auto other = mCurrentScene->EntityFromID(otherID);
                auto const &otherAABB = other.GetComponent<RigidBody>().GetAABB();

                if (aabb.Collapse(otherAABB)) {
                    aabb.SetCollided(true);
                }
            }

            rBody.Update(deltaTime);
            node.Move(rBody.Velocity());
        }
    }
}
