//
// Created by pierrhum on 04/05/2022.
//

#ifndef ELYS_RIGID_BODY_HPP
#define ELYS_RIGID_BODY_HPP

#include <variant>

#include <Core/Base.hpp>

#include <Physics/BoundingBox.hpp>
#include "ECS/Components/Node.hpp"

namespace Elys {
    class RigidBody {
      public:
        RigidBody() = default;
        explicit RigidBody(const Mesh& mesh);
        explicit RigidBody(glm::vec3 const &center, glm::vec3 const &size, glm::mat3 const &rotation);

        [[nodiscard]] Volume& GetVolume() { return mVolume; };
        [[nodiscard]] const Volume& GetVolume() const { return mVolume; };
        [[nodiscard]] glm::vec3& Velocity() { return mVelocity; };
        [[nodiscard]] glm::vec3 Position() const { return mPosition; }
        [[nodiscard]] glm::vec3 OldPosition() const { return mOldPosition; }
        [[nodiscard]] float InvMass() const {
            if (mass == 0.0f) {return 0.0f;}
            return 1.0f / mass;
        }

        void ForcePosition(glm::vec3 position);
        void ResetVelocity();
        void ResetForces();
        void PushConstraints(AABB* aabb);
        void SyncCollisionVolumes(Node& node, const Mesh& mesh);

        void Update(float deltaTime);
        void ApplyForces();
        void ApplyImpulse(RigidBody& other, const Geometry::CollisionManifold& M, int c);
        void AddLinearImpulse(const glm::vec3 &impulse);

        glm::mat4 InvTensor();
        virtual void AddRotationalImpulse(const glm::vec3& point,
                                          const glm::vec3& impulse);
      public:
        float mass = 0.1f, friction = 1.0f, cor = 0.0f;
        bool useGravity = false;
        bool isKinematic = false;
        bool collided = false;
      private:
        glm::vec3 mPosition{}, mOldPosition{};

        Volume mVolume{OBB{}};
        glm::vec3 mVelocity {0};
        std::vector<AABB*> mConstraints{};
        glm::vec3 mForces{};

        glm::vec3 orientation;
        glm::vec3 angVel;
        glm::vec3 torques; // Sum torques
    };
}

#endif // ELYS_RIGID_BODY_HPP
