//
// Created by Sam on 3/28/2022.
//

#ifndef ELYS_AABB_HPP
#define ELYS_AABB_HPP

#include <chrono>

#include <glm/glm.hpp>

#include <Core/Profiling.hpp>
#include <Render/BoundingBox.hpp>

using glm::vec3;

namespace Elys {
    class AABB : public BoundingBox {
      public:
        AABB(float min, float max) : lo(min, min, min), hi(max, max, max) {}
        AABB(vec3 min, float size) : lo(min), hi(min + size) {}
        AABB(vec3 min, vec3 max) : lo(min), hi(max) {}
        ~AABB() = default;

        [[nodiscard]] bool IsInFrustum(Frustum frustum, mat4 transform) const override;
        [[nodiscard]] bool IsForwardPlan(const Geometry::Plan& plan) const;

        [[nodiscard]] bool Contains(const AABB &other) const;
        [[nodiscard]] bool Contains(const vec3 &point) const;
        [[nodiscard]] bool Overlap(const AABB& other) const;
      private:
        vec3 hi, lo;
    };
}

#endif // ELYS_AABB_HPP
