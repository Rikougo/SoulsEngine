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
            auto& volume = rBody.GetVolume();
            auto const &mesh = entity.GetComponent<MeshRenderer>().mesh;

            // when position is reset using editor
            if (node.InheritedPosition() != rBody.Position())
                rBody.SetPosition(node.InheritedPosition());

            rBody.ApplyForces();

            for(auto otherID : mEntities) {
                if (otherID == id) continue;

                auto other = mCurrentScene->EntityFromID(otherID);
                auto &otherVolume = other.GetComponent<RigidBody>().GetVolume();

                if (Intersect(volume, otherVolume)) {
                    ELYS_CORE_INFO("Collision");
                }
            }

            if (mPhysicUpdate) {
                rBody.Update(deltaTime);
                node.SetPosition(rBody.Position());
            }
            // aabb.Update(node.InheritedTransform(), mesh);
        }
    }

    void PhysicSystem::AddEntity(EntityID newEntity) {
        if (mEntities.insert(newEntity).second) {
            
        }
    }
    void PhysicSystem::RemoveEntity(EntityID removedEntity) {
        if (mEntities.erase(removedEntity) > 0) {
        }
    }
}
