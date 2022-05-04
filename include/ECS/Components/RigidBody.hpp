//
// Created by pierrhum on 04/05/2022.
//

#ifndef ELYS_RIGIDBODY_HPP
#define ELYS_RIGIDBODY_HPP

#include <Render/AABB.hpp>

namespace Elys {
    class RigidBody {
      public:
        RigidBody() = default;
        RigidBody(const Mesh& mesh);

        [[nodiscard]] AABB* GetAABB() { return mBoundingBox; };
        [[nodiscard]] const glm::vec3& Velocity() { return mVelocity; };
        [[nodiscard]] const bool UseGravity() { return mUseGravity; };

        void EnableGravity() { mUseGravity = true; };
        void DisableGravity() { mUseGravity = false; };

        void Update(float deltaTime);

      private:
        AABB* mBoundingBox;
        bool mUseGravity = true;
        float mGravity = 9.81;
        vec3 mVelocity {0,0,0};
    };
}

#endif // ELYS_RIGIDBODY_HPP
