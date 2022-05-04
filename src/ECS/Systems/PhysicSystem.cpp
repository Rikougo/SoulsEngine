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
            auto& rbody = entity.GetComponent<RigidBody>();
            auto aabb = rbody.GetAABB();


            aabb->SetCollided(false);

            for(auto otherID : mEntities) {
                if (otherID == id) continue;

                auto other = mCurrentScene->EntityFromID(otherID);
                auto otheraabb = other.GetComponent<RigidBody>().GetAABB();

                if (aabb->Collapse(otheraabb)) {
                    aabb->SetCollided(true);
                }
            }

            rbody.Update(deltaTime);
            node.Move(rbody.Velocity());
        }
    }
}
