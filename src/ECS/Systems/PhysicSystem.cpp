//
// Created by sakeiru on 5/3/2022.
//

#include <ECS/Systems/PhysicSystem.hpp>

namespace Elys {
    void PhysicSystem::Update(float deltaTime) {
        for(auto id : mEntities) {
            auto entity = mCurrentScene->EntityFromID(id);

            auto &aabb = entity.GetComponent<AABB>();
            aabb.SetCollided(false);

            for(auto otherID : mEntities) {
                if (otherID == id) continue;

                auto other = mCurrentScene->EntityFromID(otherID);
                auto &otherAABB = other.GetComponent<AABB>();

                if (aabb.Collapse(otherAABB)) {
                    ELYS_CORE_INFO("Colliding : {0} x {1}", entity.GetComponent<Node>().name, other.GetComponent<Node>().name);
                    aabb.SetCollided(true);
                }
            }
        }
    }
}
