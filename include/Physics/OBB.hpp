//
// Created by pierrhum on 08/05/2022.
//

#ifndef ELYS_OBB_HPP
#define ELYS_OBB_HPP

#include "Physics/AABB.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <memory>

namespace Elys {
    class OBB {
      public:
        OBB() = default;
        explicit OBB(glm::vec3 const &center, glm::vec3 const &size = glm::vec3{1.0f}, glm::mat3 const &rotation = glm::mat3{1.0f});
        ~OBB() = default;

        [[nodiscard]] std::vector<glm::vec3> GetVertices() const { return mVertices; }
        [[nodiscard]] glm::vec3 GetPosition() const { return mCenter; }
        [[nodiscard]] glm::mat3 GetRotation() const { return mRotation; }
        void SetRotation(glm::mat3 Rotation) { mRotation = Rotation; }
        [[nodiscard]] glm::vec3 Size() const { return mSize; }
        [[nodiscard]] glm::vec3& Size() { return mSize; }
        [[nodiscard]] std::shared_ptr<VertexArray> VAO() { return mVAO; }

        void UpdateBuffers();
        void Update(glm::vec3 const &center, glm::mat3 const &rotation);

        std::pair<float, float> GetInterval(glm::vec3 const &axis) const;
        static Geometry::CollisionManifold FindCollisionFeatures(const OBB& A,
                                                const OBB& B);
        std::vector<glm::vec3> ClipEdgesToOBB(
            const std::vector<Geometry::Line>& edges) const;
        std::vector<Geometry::Line> GetEdges() const;
      private:
        void UpdateVertices();
        std::vector<Geometry::Plan> GetPlanes() const;
        static float PenetrationDepth(const OBB& o1, const OBB& o2,
                               const glm::vec3& axis, bool* outShouldFlip);
      public:
        static bool CollapseOnAxis(OBB const &left, OBB const &right, glm::vec3 const &axis);
        static bool Collapse(OBB const &left, OBB const &right);
        bool PointInOBB(const glm::vec3& point) const;
      private:
        mutable std::shared_ptr<VertexArray> mVAO;

        std::vector<glm::vec3> mVertices;
        glm::vec3 mCenter{0.0f, 0.0f, 0.0f};
        glm::vec3 mSize{1.0f, 1.0f, 1.0f};
        glm::mat3 mRotation{1.0f};
    };
}
#endif // ELYS_OBB_HPP
