//
// Created by Sam on 3/28/2022.
//

#include "Physics/AABB.hpp"

#include "glad/glad.h"

using glm::vec3;

namespace Elys {
    AABB::AABB() : lo(0), hi(0) { UpdateVertices(); }
    AABB::AABB(float min, float max) : lo(min), hi(max) { UpdateVertices(); }
    AABB::AABB(vec3 min, float size) : lo(min), hi(min + size) { UpdateVertices(); }
    AABB::AABB(vec3 min, vec3 max) : lo(min), hi(max) { UpdateVertices(); }
    // https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
    AABB::AABB(const Mesh &mesh) {
        lo = vec3(std::numeric_limits<float>::max());
        hi = vec3(std::numeric_limits<float>::min());

        for (Vertex v : mesh.Vertices()) {
            if (v.position.x < lo.x)
                lo.x = v.position.x;
            if (v.position.y < lo.y)
                lo.y = v.position.y;
            if (v.position.z < lo.z)
                lo.z = v.position.z;

            if (v.position.x > hi.x)
                hi.x = v.position.x;
            if (v.position.y > hi.y)
                hi.y = v.position.y;
            if (v.position.z > hi.z)
                hi.z = v.position.z;
        }

        UpdateVertices();
    }

    void AABB::UpdateBuffers() {
        if (mVAO) {
            mVAO->GetVertexBuffer()->SetData((void *)mVertices.data(),
                                             static_cast<uint32_t>(mVertices.size() * sizeof(vec3)),
                                             GL_DYNAMIC_DRAW);
        } else {
            mVAO = std::make_shared<VertexArray>();
            auto vbo = std::make_shared<VertexBuffer>(
                (void *)mVertices.data(), static_cast<uint32_t>(mVertices.size() * sizeof(vec3)),
                GL_DYNAMIC_DRAW);
            BufferLayout vertexLayout{{"position", sizeof(vec3), 3, GL_FLOAT}};
            vbo->SetLayout(vertexLayout);
            mVAO->SetVertexBuffer(vbo);
        }
    }

    void AABB::Update(glm::mat4 transform, Mesh const &mesh) {
        if (transform == mTransform)
            return;

        lo = vec3(std::numeric_limits<float>::max());
        hi = vec3(-std::numeric_limits<float>::max());

        mTransform = glm::mat4(transform);
        for (Vertex rawV : mesh.Vertices()) {
            glm::vec3 v = mTransform * glm::vec4(rawV.position, 1.0f);
            if (v.x < lo.x)
                lo.x = v.x;
            if (v.y < lo.y)
                lo.y = v.y;
            if (v.z < lo.z)
                lo.z = v.z;

            if (v.x > hi.x)
                hi.x = v.x;
            if (v.y > hi.y)
                hi.y = v.y;
            if (v.z > hi.z)
                hi.z = v.z;
        }

        UpdateVertices();
    }

    void AABB::UpdateVertices() {
        mVertices = {// FRONT
             lo, vec3(hi.x, lo.y, lo.z),
             vec3(hi.x, lo.y, lo.z), vec3(hi.x, hi.y, lo.z),
             vec3(hi.x, hi.y, lo.z), vec3(lo.x, hi.y, lo.z),
             vec3(lo.x, hi.y, lo.z), lo,

             // SIDE
             lo, vec3(lo.x, lo.y, hi.z),
             vec3(hi.x, lo.y, lo.z), vec3(hi.x, lo.y, hi.z),
             vec3(hi.x, hi.y, lo.z), hi,
             vec3(lo.x, hi.y, lo.z), vec3(lo.x, hi.y, hi.z),

             // BACK
             vec3(hi.x, lo.y, hi.z), vec3(lo.x, lo.y, hi.z),
             vec3(lo.x, lo.y, hi.z), vec3(lo.x, hi.y, hi.z),
             vec3(lo.x, hi.y, hi.z), hi,
             hi, vec3(hi.x, lo.y, hi.z)
        };
    }

    bool AABB::Collapse(AABB const &left, AABB const &right) {
        return (left.lo.x <= right.hi.x && left.hi.x >= right.lo.x) &&
               (left.lo.y <= right.hi.y && left.hi.y >= right.lo.y) &&
               (left.lo.z <= right.hi.z && left.hi.z >= right.lo.z);
    }

    bool AABB::Linetest(const AABB &aabb, const Geometry::Line &line) {
        auto direction = line.End() - line.Start();
        Geometry::Ray ray{line.Start(), glm::normalize(direction)};

        Geometry::RaycastResult result;
        Raycast(aabb, ray, result);
        return result.hit && result.t >= 0 && result.t <= line.Length();
    }

    bool AABB::Raycast(const AABB &aabb, Geometry::Ray const &ray, Geometry::RaycastResult &result) {
        glm::vec3 min(aabb.lo), max(aabb.hi);

        // NOTE: Any component of direction could be 0!
        // to avoid a division by 0, you need to add
        // additional safety checks.
        std::array<float, 6> tValues{0.0f};

        tValues[0] = (min.x - ray.Origin().x) / ray.Direction().x;
        tValues[1] = (max.x - ray.Origin().x) / ray.Direction().x;
        tValues[2] = (min.y - ray.Origin().y) / ray.Direction().y;
        tValues[3] = (max.y - ray.Origin().y) / ray.Direction().y;
        tValues[4] = (min.z - ray.Origin().z) / ray.Direction().z;
        tValues[5] = (max.z - ray.Origin().z) / ray.Direction().z;

        float tMin = std::max({
            std::min({tValues[0], tValues[1]}),
            std::min({tValues[2], tValues[3]}),
            std::min({tValues[4], tValues[5]})
        });

        float tMax = std::min({
            std::max({tValues[0], tValues[1]}),
            std::max({tValues[2], tValues[3]}),
            std::max({tValues[4], tValues[5]})
        });

        if (tMax < 0 || tMin > tMax) {
            result.hit = false;
            return false;
        }

        static constexpr glm::vec3 normals[] = {
            {-1, 0, 0}, {1, 0, 0},
            {0, -1, 0}, {0, 1, 0},
            {0, 0, -1}, {0, 0, 1}
        };

        auto t = tMin < 0.0f ? tMax : tMin;

        result.t = t;
        result.point = ray.Origin() + ray.Direction() * t;
        result.hit = true;

        result.normal = vec3(0);
        for(uint8_t i = 0; i < 6; i++) {
            if (tValues[i] == t) {
                result.normal = normals[i];
            }
        }

        return true;
    }
} // namespace Elys