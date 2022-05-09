//
// Created by pierrhum on 04/05/2022.
//

#ifndef ELYS_RIGID_BODY_HPP
#define ELYS_RIGID_BODY_HPP

#include <variant>

#include <Physics/BoundingBox.hpp>

namespace Elys {
    class RigidBody {
      public:
        RigidBody() = default;
        explicit RigidBody(const Mesh& mesh);
        explicit RigidBody(glm::vec3 const &center, glm::vec3 const &size, glm::mat3 const &rotation);

        [[nodiscard]] Volume& GetVolume() { return mBoundingBox; };
        [[nodiscard]] glm::vec3& Velocity() { return mVelocity; };
        [[nodiscard]] bool UseGravity() const { return useGravity; };
        [[nodiscard]] glm::vec3 Position() { return mPosition; }
        [[nodiscard]] glm::vec3 OldPosition() { return mOldPosition; }

        void SetUseGravity(bool value) { useGravity = value; }

        void SetPosition(glm::vec3 position);
        void ResetVelocity();
        void PushConstraints(AABB* aabb);

        void Update(float deltaTime);
        void ApplyForces();
        void AddLinearImpulse(const glm::vec3 impulse);
        void SolveConstraints();

        glm::mat4 InvTensor();
        virtual void AddRotationalImpulse(const glm::vec3& point,
                                          const glm::vec3& impulse);
      public:
        float mass = 0.1f, bounce = 1.0f, friction = 1.0f;
        bool useGravity = false;
      private:
        glm::vec3 mPosition{}, mOldPosition{};

        Volume mBoundingBox{};
        glm::vec3 mVelocity {0};
        std::vector<AABB*> mConstraints{};
        glm::vec3 mForces{};
        bool mCollided{false};

        glm::vec3 orientation;
        glm::vec3 angVel;
        glm::vec3 torques; // Sum torques
    };
}

#endif // ELYS_RIGID_BODY_HPP
