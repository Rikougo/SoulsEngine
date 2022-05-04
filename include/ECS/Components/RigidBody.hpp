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
        void Update();

      private:
        AABB* mBoundingBox;
        float mGravity = 9.81;
    };
}

#endif // ELYS_RIGIDBODY_HPP
