//
// Created by pierrhum on 08/05/2022.
//
#include "Physics/BoundingBox.hpp"

#include <glad/glad.h>

using glm::vec3;

namespace Elys {
    // Update buffers impl
    namespace {
        void Volume_UpdateBuffersImpl(AABB &aabb) { aabb.UpdateBuffers(); }

        void Volume_UpdateBuffersImpl(OBB &obb) { obb.UpdateBuffers(); }
    } // namespace

    void UpdateBuffers(Volume &volume) {
        std::visit([&](auto &x) { Volume_UpdateBuffersImpl(x); }, volume);
    }

    // Draw volume impl
    namespace {
        void Volume_DrawVolumeImpl(AABB &aabb) {
            auto VAO = aabb.VAO();
            VAO->Bind();
            glDrawArrays(GL_LINES, 0, 24);
            VAO->Unbind();
        }

        void Volume_DrawVolumeImpl(OBB &obb) {
            auto VAO = obb.VAO();
            VAO->Bind();
            glDrawElements(GL_LINES, (GLsizei)VAO->GetIndexBuffer()->Size(), GL_UNSIGNED_INT, nullptr);
            VAO->Unbind();
        }
    }

    void DrawVolume(Volume &volume) {
        std::visit([&](auto &x) { Volume_DrawVolumeImpl(x); }, volume);
    }

    // Intersect impl
    namespace {
        Geometry::CollisionManifold Volume_IntersectImpl(AABB &left, AABB &right) {
            /*
            auto [leftLo, leftHi] = left.GetBounds();
            auto [rightLo, rightHi] = right.GetBounds();


            return (leftLo.x <= rightHi.x && leftHi.x >= rightLo.x) &&
               (leftLo.y <= rightHi.y && leftHi.y >= rightLo.y) &&
               (leftLo.z <= rightHi.z && leftHi.z >= rightLo.z);
            */
            Geometry::CollisionManifold result;
            ResetCollisionManifold(&result);
            return result;
        }

        Geometry::CollisionManifold Volume_IntersectImpl(AABB &left, OBB &right) {
            Geometry::CollisionManifold result;
            ResetCollisionManifold(&result);
            return result;
        }

        Geometry::CollisionManifold Volume_IntersectImpl(OBB &left, AABB &right) {
            return Volume_IntersectImpl(right, left);
        }

        Geometry::CollisionManifold Volume_IntersectImpl(OBB &left, OBB &right) {
            return OBB::FindCollisionFeatures(left, right);
        }
    }

    Geometry::CollisionManifold Intersect(Volume &left, Volume &right) {
        return std::visit<Geometry::CollisionManifold>([&](auto &x, auto &y) { return Volume_IntersectImpl(x, y); }, left, right);
    }
} // namespace Elys