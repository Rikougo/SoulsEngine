//
// Created by Sam on 3/28/2022.
//

#ifndef ELYS_AABB_HPP
#define ELYS_AABB_HPP

#include <chrono>
#include <array>

#include <glm/glm.hpp>

#include <Core/Profiling.hpp>
#include <Core/Geometry.hpp>

#include <Render/DataHolder.hpp>
#include <Render/Mesh.hpp>
#include <Render/Camera.hpp>

using std::array;
using glm::vec3;

namespace Elys {
    class AABB {
      public:
        AABB();
        AABB(float min, float max);
        AABB(vec3 min, float size);
        AABB(vec3 min, vec3 max);
        explicit AABB(const Mesh& mesh);
        ~AABB() = default;

        [[nodiscard]] bool Collapse(AABB const &other) const;
        [[nodiscard]] bool IsInFrustum(Frustum frustum, glm::mat4 transform) const;
        [[nodiscard]] bool IsForwardPlan(const Geometry::Plan& plan) const;

        [[nodiscard]] std::shared_ptr<VertexArray> VAO() const { return mVAO; }
        [[nodiscard]] std::vector<vec3> Vertices() const { return mVertices; }

        void Update(glm::mat4 transform, Mesh const &mesh);
        void SetCollided(bool value) { mCollided = value; }
        [[nodiscard]] bool IsCollided() const { return mCollided; }
      private:
        void InitBuffers();
        void UpdateVertices();
      private:
        mutable std::shared_ptr<VertexArray> mVAO;
        bool mCollided = false;
        std::vector<glm::vec3> mVertices;
        glm::mat4 mTransform{}; // transform cache
        glm::vec3 hi{}, lo{};
    };
}

#endif // ELYS_AABB_HPP
