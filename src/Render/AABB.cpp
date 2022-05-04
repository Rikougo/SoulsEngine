//
// Created by Sam on 3/28/2022.
//

#include <Render/AABB.hpp>
#include <Render/Mesh.hpp>

#include <glad/glad.h>

namespace Elys {
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

    bool AABB::IsInFrustum(Frustum frustum, mat4 transform) const {
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

    void AABB::UpdateBuffers() const {
        mVBO->SetData(mVertices.data(), mVertices.size(), GL_DYNAMIC_DRAW);
        mVAO->UpdateVertexBuffer();
    }

    void AABB::Update(glm::mat4 transform, Mesh const &mesh) {
        bool test = transform == mTransform;
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
        ELYS_CORE_INFO("Update AABB : {0} {1} {2}", hi.x, hi.y, hi.z);

        auto vLO = lo - vec3(0.001f);
        auto vHI = hi + vec3(0.001f);

        mVertices = {
            // FRONT
            vLO,
            vec3(vHI.x, vLO.y, vLO.z),

            vec3(vHI.x, vLO.y, vLO.z),
            vec3(vHI.x, vHI.y, vLO.z),

            vec3(vHI.x, vHI.y, vLO.z),
            vec3(vLO.x, vHI.y, vLO.z),

            vec3(vLO.x, vHI.y, vLO.z),
            vLO,

            // SIDE
            vLO,
            vec3(vLO.x, vLO.y, vHI.z),

            vec3(vHI.x, vLO.y, vLO.z),
            vec3(vHI.x, vLO.y, vHI.z),

            vec3(vHI.x, vHI.y, vLO.z),
            vHI,

            vec3(vLO.x, vHI.y, vLO.z),
            vec3(vLO.x, vHI.y, vHI.z),

            // BACK
            vec3(vHI.x, vLO.y, vHI.z),
            vec3(vLO.x, vLO.y, vHI.z),

            vec3(vLO.x, vLO.y, vHI.z),
            vec3(vLO.x, vHI.y, vHI.z),

            vec3(vLO.x, vHI.y, vHI.z),
            vHI,

            vHI,
            vec3(vHI.x, vLO.y, vHI.z)
        };

        mVAO.reset();
        mVBO.reset();
        mVAO = std::make_shared<VertexArray>();
        mVBO = std::make_shared<VertexBuffer>((void*)&mVertices[0], mVertices.size() * sizeof(Vertex), GL_DYNAMIC_DRAW);
        BufferLayout vertexLayout{ {"position", sizeof(vec3), 3, GL_FLOAT} };
        mVBO->SetLayout(vertexLayout);
        mVAO->AddVertexBuffer(mVBO);
    }

    bool AABB::Collapse(const AABB *other) const {
        ELYS_CORE_INFO("Collapse AABB : {0} {1} {2} - Other {3} {4} {5}", hi.x, hi.y, hi.z, other->hi.x, other->hi.y, other->hi.z);
        return (lo.x <= other->hi.x && hi.x >= other->lo.x) &&
               (lo.y <= other->hi.y && hi.y >= other->lo.y) &&
               (lo.z <= other->hi.z && hi.z >= other->lo.z);
    }
}