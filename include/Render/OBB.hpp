//
// Created by pierrhum on 08/05/2022.
//

#ifndef ELYS_OBB_HPP
#define ELYS_OBB_HPP

#include "Mesh.hpp"
#include "AABB.hpp"

using glm::vec3;
using glm::mat3;

namespace Elys {
    class OBB : BoundingBox {
      public:
        OBB() = default;
        explicit OBB(const Mesh& mesh);
        ~OBB() = default;

        void Update(glm::mat4 transform, Mesh const &mesh) override;

      private:
        void UpdateVertices() override;
      private:
        vec3 center;
        vec3 mUp, mForward, mRight;
        mat3 mRotation;
    };
}
#endif // ELYS_OBB_HPP
