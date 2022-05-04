//
// Created by sakeiru on 5/3/2022.
//

#include <ECS/Systems/PhysicSystem.hpp>

namespace Elys {
    void PhysicSystem::Update(float deltaTime) {
        for(auto id : mEntities) {
            auto entity = mCurrentScene->EntityFromID(id);

            auto aabb = entity.GetComponent<RigidBody>().GetAABB();
            aabb->SetCollided(false);

            for(auto otherID : mEntities) {
                if (otherID == id) continue;

                auto other = mCurrentScene->EntityFromID(otherID);
                auto otheraabb = other.GetComponent<RigidBody>().GetAABB();

                if (aabb->Collapse(otheraabb)) {
                    ELYS_CORE_INFO("Colliding : {0} x {1}", entity.GetComponent<Node>().name, other.GetComponent<Node>().name);
                    aabb->SetCollided(true);
                }
            }
        }
    }
}
