//
// Created by sakeiru on 5/3/2022.
//

#include <ECS/Systems/PhysicSystem.hpp>

namespace Elys {
    namespace {
        template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
        // explicit deduction guide (not needed as of C++20)
        template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
    } // namespace

    void PhysicSystem::Update(float deltaTime) {
        for (auto id : mEntities) {
            auto entity = mCurrentScene->EntityFromID(id);

            // PHYSICS
            auto &node = entity.GetComponent<Node>();
            auto &rBody = entity.GetComponent<RigidBody>();
            auto &volume = rBody.GetVolume();
            auto const &mesh = entity.GetComponent<MeshRenderer>().mesh;

            // when position is reset using editor
            if (node.InheritedPosition() != rBody.Position())
                rBody.SetPosition(node.InheritedPosition());

            rBody.ApplyForces();

            for (auto otherID : mEntities) {
                if (otherID == id)
                    continue;

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

            std::visit(
                overloaded{
                    [&](AABB &aabb) { aabb.Update(node.InheritedTransform(), mesh); },
                    [&](OBB &obb) { obb.Update(node.InheritedPosition(), node.InheritedScale(), glm::mat3_cast(node.InheritedRotation())); }
                },
                volume
            );
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
} // namespace Elys
