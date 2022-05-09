//
// Created by Sam on 3/28/2022.
//

#ifndef ELYS_AABB_HPP
#define ELYS_AABB_HPP

#include <chrono>
#include <array>

#include "Core/Profiling.hpp"
#include "Core/Geometry.hpp"

#include <Render/Mesh.hpp>
#include <Render/Camera.hpp>
#include <Render/Mesh.hpp>

namespace Elys {
    class AABB {
      public:
        AABB();
        AABB(float min, float max);
        AABB(glm::vec3 min, float size);
        AABB(glm::vec3 min, glm::vec3 max);
        explicit AABB(const Mesh& mesh);
        ~AABB() = default;

        [[nodiscard]] std::shared_ptr<VertexArray> VAO() { return mVAO; }
        [[nodiscard]] float Size() const { return (hi - lo).x; }
        [[]]
        std::pair<glm::vec3, glm::vec3> GetBounds() { return {lo, hi}; }

        void UpdateVertices();
        void Update(glm::mat4 transform, Mesh const &mesh);
      private:
        glm::vec3 hi{}, lo{};
        std::vector<glm::vec3> mRenderVertices;
        glm::mat4 mTransform{}; // transform cache
        mutable std::shared_ptr<VertexArray> mVAO;
      public:
        [[nodiscard]] static bool Collapse(AABB const &left, AABB const &right);
        [[nodiscard]] static bool Linetest(AABB const& aabb, Geometry::Line const &line);
        [[nodiscard]] static bool Raycast(AABB const& aabb, Geometry::Ray const &ray, Geometry::RaycastResult &result);
    };
}

#endif // ELYS_AABB_HPP
