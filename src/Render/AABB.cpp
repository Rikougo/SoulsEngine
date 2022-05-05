//
// Created by Sam on 3/28/2022.
//

#include <Render/AABB.hpp>

#include <glad/glad.h>

using glm::vec3;

namespace Elys {
    AABB::AABB() : lo(0), hi(0) {}
    AABB::AABB(float min, float max) : lo(min), hi(max) {}
    AABB::AABB(vec3 min, float size) : lo(min), hi(min + size) {}
    AABB::AABB(vec3 min, vec3 max) : lo(min), hi(max) {}
    // https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
    AABB::AABB(const Mesh& mesh) {
        lo = vec3(std::numeric_limits<float>::max());
        hi = vec3(std::numeric_limits<float>::min());

        for(Vertex v : mesh.Vertices())
        {
            if(v.position.x < lo.x) lo.x = v.position.x;
            if(v.position.y < lo.y) lo.y = v.position.y;
            if(v.position.z < lo.z) lo.z = v.position.z;

            if(v.position.x > hi.x) hi.x = v.position.x;
            if(v.position.y > hi.y) hi.y = v.position.y;
            if(v.position.z > hi.z) hi.z = v.position.z;
        }

        InitBuffers();
    }

    void AABB::InitBuffers() {
        UpdateVertices();

        mVAO.reset();
        mVAO = std::make_shared<VertexArray>();
        auto vbo = std::make_shared<VertexBuffer>((void*)mVertices.data(), static_cast<uint32_t>(mVertices.size() * sizeof(vec3)), GL_DYNAMIC_DRAW);
        BufferLayout vertexLayout{ {"position", sizeof(vec3), 3, GL_FLOAT} };
        vbo->SetLayout(vertexLayout);
        mVAO->SetVertexBuffer(vbo);
    }

    void AABB::UpdateVertices() {
        mVertices = {
            // FRONT
            lo,
            vec3(hi.x, lo.y, lo.z),

            vec3(hi.x, lo.y, lo.z),
            vec3(hi.x, hi.y, lo.z),

            vec3(hi.x, hi.y, lo.z),
            vec3(lo.x, hi.y, lo.z),

            vec3(lo.x, hi.y, lo.z),
            lo,

            // SIDE
            lo,
            vec3(lo.x, lo.y, hi.z),

            vec3(hi.x, lo.y, lo.z),
            vec3(hi.x, lo.y, hi.z),

            vec3(hi.x, hi.y, lo.z),
            hi,

            vec3(lo.x, hi.y, lo.z),
            vec3(lo.x, hi.y, hi.z),

            // BACK
            vec3(hi.x, lo.y, hi.z),
            vec3(lo.x, lo.y, hi.z),

            vec3(lo.x, lo.y, hi.z),
            vec3(lo.x, hi.y, hi.z),

            vec3(lo.x, hi.y, hi.z),
            hi,

            hi,
            vec3(hi.x, lo.y, hi.z)
        };
    }

    bool AABB::IsInFrustum(Frustum frustum, glm::mat4 transform) const {
        vec3 tMin = transform * glm::vec4(lo, 1.0f), tMax = transform * glm::vec4(hi, 1.0f);
        AABB transformedAABB(tMin, tMax);

        return transformedAABB.IsForwardPlan(frustum.topFace) &&
            transformedAABB.IsForwardPlan(frustum.bottomFace) &&
            transformedAABB.IsForwardPlan(frustum.leftFace) &&
            transformedAABB.IsForwardPlan(frustum.rightFace) &&
            transformedAABB.IsForwardPlan(frustum.nearFace) &&
            transformedAABB.IsForwardPlan(frustum.farFace);
    }

    bool AABB::IsForwardPlan(const Geometry::Plan& plan) const {
        vec3 center = (hi + lo) * 0.5f;
        vec3 extent = hi - center;

        float r = extent.x * std::abs(plan.normal.x) + extent.y + std::abs(plan.normal.y) + extent.z * std::abs(plan.normal.z);
        float signedDistance = plan.GetSignedDistance(center);
        return -r <= signedDistance;
    }

    void AABB::Update(glm::mat4 transform, Mesh const &mesh) {
        if (transform == mTransform) return;

        lo = vec3(std::numeric_limits<float>::max());
        hi = vec3(-std::numeric_limits<float>::max());

        mTransform = glm::mat4(transform);
        for(Vertex rawV : mesh.Vertices())
        {
            glm::vec3 v = mTransform * glm::vec4(rawV.position, 1.0f);
            if(v.x < lo.x) lo.x = v.x;
            if(v.y < lo.y) lo.y = v.y;
            if(v.z < lo.z) lo.z = v.z;
            
            if(v.x > hi.x) hi.x = v.x;
            if(v.y > hi.y) hi.y = v.y;
            if(v.z > hi.z) hi.z = v.z;
        }

        if (mVAO) {
            UpdateVertices();

            // TODO : Improve the way to replace VAO data.
            mVAO->GetVertexBuffer()->SetData((void*)mVertices.data(), static_cast<uint32_t>(mVertices.size() * sizeof(vec3)), GL_DYNAMIC_DRAW);
        } else {
            InitBuffers();
        }
    }

    bool AABB::Collapse(const AABB &other) const {
        return (lo.x <= other.hi.x && hi.x >= other.lo.x) &&
               (lo.y <= other.hi.y && hi.y >= other.lo.y) &&
               (lo.z <= other.hi.z && hi.z >= other.lo.z);
    }
}