//
// Created by sakeiru on 5/3/2022.
//

#include <ECS/Systems/PhysicSystem.hpp>

namespace Elys {
    void PhysicSystem::Update(float deltaTime) {
        if (mPhysicUpdate) {

            results.clear();
            colliders1.clear();
            colliders2.clear();

            std::vector<Entity> entities;
            for (auto id : mEntities) {
                entities.push_back(mCurrentScene->EntityFromID(id));
            }

            for (int i = 0; i < entities.size(); i++) {
                auto entity = entities[i];

                // PHYSICS
                auto &node = entity.GetComponent<Node>();
                auto &rBody = entity.GetComponent<RigidBody>();
                auto &volume = rBody.GetVolume();
                auto const &mesh = entity.GetComponent<MeshRenderer>().mesh;

                // when position is reset using editor
                if (node.InheritedPosition() != rBody.Position())
                    rBody.SetPosition(node.InheritedPosition());

                for (int j = i+1; j < entities.size(); j++) {
                    auto other = entities[j];
                    if (other == entity)
                        continue;

                    auto otherRBody = other.GetComponent<RigidBody>();
                    auto &otherVolume = otherRBody.GetVolume();

                    Geometry::CollisionManifold result;
                    ResetCollisionManifold(&result);

                    result = Intersect(volume, otherVolume);
                    if (result.colliding) {
                        colliders1.push_back(&rBody);
                        colliders2.push_back(&otherRBody);
                        results.push_back(result);
                    }
                }
            }
            // Calculate forces acting on the object
            for (int i = 0; i < entities.size(); i++) {
                auto entity = entities[i];
                auto &rBody = entity.GetComponent<RigidBody>();
                rBody.ApplyForces();
            }
            // Then, we apply an impulse to objects that are colliding to correct these collisions
            for (int k = 0; k <ImpulseIteration; ++k) {
                for (int i = 0; i < results.size(); ++i) {
                    int jSize = results[i].contacts.size();
                    for (int j = 0; j <jSize; ++j) {
                        RigidBody* m1 = (RigidBody*)colliders1[i];
                        RigidBody* m2 = (RigidBody*)colliders2[i];
                        m1->ApplyImpulse(*m2, results[i], j);
                    }
                }
            }
            // Integrate the forces and velocity of every rigidbody
            for (int i = 0; i < entities.size(); i++) {
                auto entity = entities[i];
                auto &node = entity.GetComponent<Node>();
                auto &rBody = entity.GetComponent<RigidBody>();
                auto &volume = rBody.GetVolume();
                auto const &mesh = entity.GetComponent<MeshRenderer>().mesh;

                rBody.Update(deltaTime);
                rBody.SynchCollisionVolumes(node, mesh);
                node.SetPosition(rBody.Position());
            }

            // Perform Linear Projection to fix any sinking issues
            for (int i = 0, size = results.size(); i< size; ++i) {
                RigidBody* m1 = (RigidBody*)colliders1[i];
                RigidBody* m2 = (RigidBody*)colliders2[i];
                float totalMass = m1->InvMass() + m2->InvMass();

                if (totalMass == 0.0f) {
                    continue;
                }

                float depth = fmaxf(results[i].depth - PenetrationSlack, 0.0f);
                float scalar = depth / totalMass;
                glm::vec3 correction = results[i].normal * scalar * LinearProjectionPercent;
                ELYS_CORE_INFO("{0}, {1}, {2}", correction.x, correction.y, correction.z);
                if(!m1->isKinematic)
                    m1->SetPosition(m1->Position() - correction * m1->InvMass());
                if(!m2->isKinematic)
                    m2->SetPosition(m2->Position() + correction * m2->InvMass());
                for (int i = 0; i < entities.size(); i++) {
                    auto entity = entities[i];
                    auto &node = entity.GetComponent<Node>();
                    auto const &mesh = entity.GetComponent<MeshRenderer>().mesh;
                    auto &rBody = entity.GetComponent<RigidBody>();
                    if(!m1->isKinematic && &rBody==m1) {
                        m1->SynchCollisionVolumes(node, mesh);
                        node.SetPosition(rBody.Position());
                    } else if(!m2->isKinematic && &rBody==m2) {
                        m2->SynchCollisionVolumes(node, mesh);
                        node.SetPosition(rBody.Position());
                    }

                }
                // synch collider & check solveconstraints

            }

            for (auto id : mEntities) {
                auto entity = mCurrentScene->EntityFromID(id);
                auto &rBody = entity.GetComponent<RigidBody>();
                rBody.SolveConstraints();
            }

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
