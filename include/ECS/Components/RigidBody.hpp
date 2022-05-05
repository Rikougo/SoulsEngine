//
// Created by pierrhum on 04/05/2022.
//

#ifndef ELYS_RIGID_BODY_HPP
#define ELYS_RIGID_BODY_HPP

#include <Render/AABB.hpp>

namespace Elys {
    class RigidBody {
      public:
        RigidBody() = default;
        RigidBody(const Mesh& mesh);

        [[nodiscard]] AABB& GetAABB() { return mBoundingBox; };
        [[nodiscard]] glm::vec3& Velocity() { return mVelocity; };
        [[nodiscard]] bool UseGravity() const { return mUseGravity; };

        void SetUseGravity(bool value) { mUseGravity = value; }

        void Update(float deltaTime);
        void ApplyForce(glm::vec3 force, float dt);

      private:
        AABB mBoundingBox{};
        bool mUseGravity = true;
        bool mBounce = true;
        float mMass = 0.05f;
        float mGravity = 9.81f;
        glm::vec3 mVelocity {0,0,0};
    };
}

#endif // ELYS_RIGID_BODY_HPP
