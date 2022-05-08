//
// Created by pierrhum on 08/05/2022.
//

#ifndef ELYS_OBB_HPP
#define ELYS_OBB_HPP

#include <Render/Mesh.hpp>
#include <Render/AABB.hpp>

namespace Elys {
    class OBB : BoundingBox {
      public:
        OBB() = default;
        OBB(glm::vec3 const &center, glm::vec3 const &size = glm::vec3{1.0f}, glm::mat3 const &rotation = glm::mat3{1.0f});
        ~OBB() = default;

        void Update(glm::vec3 const &center, glm::vec3 const &size, glm::mat3 const &rotation);

      private:
        void UpdateVertices() override;
      private:
        glm::vec3 mSize{1.0f, 1.0f, 1.0f};
        glm::vec3 mCenter{0.0f, 0.0f, 0.0f};
        glm::mat3 mRotation{1.0f};
      public:
        static std::pair<float, float> GetInterval(OBB const &obb, glm::vec3 const &axis);
        static bool CollapseOnAxis(OBB const &left, OBB const &right, glm::vec3 const &axis);
        static bool Collapse(OBB const &left, OBB const &right);
    };
}
#endif // ELYS_OBB_HPP
