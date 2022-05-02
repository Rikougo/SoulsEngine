//
// Created by Sam on 3/28/2022.
//

#include <Render/AABB.hpp>
#include <Render/Mesh.hpp>

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
        GenerateBuffers();
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
    vector<vec3> AABB::Vertices() const {
        return {
            lo,
            vec3(hi.x, lo.y, lo.z),
            vec3(hi.x, lo.y, lo.z),
            vec3(hi.x, hi.y, lo.z),
            vec3(hi.x, hi.y, lo.z),
            vec3(lo.x, hi.y, lo.z),
            vec3(lo.x, hi.y, lo.z),
            vec3(lo.x, lo.y, hi.z),
            vec3(lo.x, lo.y, hi.z),
            vec3(hi.x, lo.y, hi.z),
            vec3(hi.x, lo.y, hi.z),
            hi,
            vec3(lo.x, hi.y, hi.z)
        };
    }
}