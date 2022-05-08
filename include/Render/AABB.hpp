//
// Created by Sam on 3/28/2022.
//

#ifndef ELYS_AABB_HPP
#define ELYS_AABB_HPP

#include <chrono>
#include <array>

#include <Core/Profiling.hpp>
#include <Core/Geometry.hpp>

#include <Render/DataHolder.hpp>
#include <Render/Mesh.hpp>
#include <Render/Camera.hpp>
#include "BoundingBox.hpp"

namespace Elys {
    class AABB : public BoundingBox {
      public:
        AABB();
        AABB(float min, float max);
        AABB(glm::vec3 min, float size);
        AABB(glm::vec3 min, glm::vec3 max);
        explicit AABB(const Mesh& mesh);
        ~AABB() = default;

        [[nodiscard]] bool Collapse(AABB const &other) const;
        [[nodiscard]] bool IsInFrustum(Frustum frustum, glm::mat4 transform) const;
        [[nodiscard]] bool IsForwardPlan(const Geometry::Plan& plan) const;

        [[nodiscard]] float Size() const { return (hi - lo).x; }

        void Update(glm::mat4 transform, Mesh const &mesh) override;
      private:
        void UpdateVertices() override;
      private:
        glm::vec3 hi{}, lo{};

      public:
        static bool Linetest(AABB const& aabb, Geometry::Line const &line);
        static bool Raycast(AABB const& aabb, Geometry::Ray const &ray, Geometry::RaycastResult &result);
    };
}

#endif // ELYS_AABB_HPP
