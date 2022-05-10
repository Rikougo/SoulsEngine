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
        [[nodiscard]] glm::vec3& Velocity() { return mVelocity; };
        [[nodiscard]] bool UseGravity() const { return useGravity; };
        [[nodiscard]] glm::vec3 Position() { return mPosition; }
        [[nodiscard]] glm::vec3 Orientation() { return mOrientation; }
        [[nodiscard]] glm::vec3 OldPosition() { return mOldPosition; }

        void SetUseGravity(bool value) { useGravity = value; }
        void SetIsKinematic(bool value) { isKinematic = value; }

        void SetPosition(glm::vec3 position);
        void ResetVelocity();
        void PushConstraints(AABB* aabb);
        void SyncCollisionVolumes(Node& node, const Mesh& mesh);

        float InvMass() {
            if (mass == 0.0f) {return 0.0f;}
            return 1.0f / mass;
        }
        void Update(float deltaTime);
        void ApplyForces();
        void ApplyImpulse(RigidBody& other, const Geometry::CollisionManifold& M, int c);
        void AddLinearImpulse(const glm::vec3 &impulse);
        void SolveConstraints();

        glm::mat4 InvTensor();
        virtual void AddRotationalImpulse(const glm::vec3& point,
                                          const glm::vec3& impulse);
      public:
        float mass = 0.1f, bounce = 1.0f, friction = 1.0f, cor = 0.5f;
        bool useGravity = false;
        bool isKinematic = false;
      private:
        glm::vec3 mPosition{}, mOldPosition{};

        Volume mVolume{OBB{}};
        glm::vec3 mVelocity {0};
        std::vector<AABB*> mConstraints{};
        glm::vec3 mForces{};
        bool mCollided{false};

        glm::vec3 mOrientation {0};
        glm::vec3 angVel{0};
        glm::vec3 torques{0}; // Sum torques
    };
}

#endif // ELYS_RIGID_BODY_HPP
