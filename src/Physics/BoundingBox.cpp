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
            /*
            auto lRotation = left.GetRotation(), rRotation = right.GetRotation();

            std::array<glm::vec3, 15> testAxis = {
                lRotation[0], lRotation[1], lRotation[2],
                rRotation[0], rRotation[1], rRotation[2]
            };

            for(uint8_t i = 0; i < 3; i++) {
                testAxis[6 + i * 3 + 0] = glm::cross(testAxis[i], testAxis[3]);
                testAxis[6 + i * 3 + 1] = glm::cross(testAxis[i], testAxis[4]);
                testAxis[6 + i * 3 + 2] = glm::cross(testAxis[i], testAxis[5]);
            }

            for(auto const &axis : testAxis) {
                auto a = left.GetInterval(axis);
                auto b = right.GetInterval(axis);

                if (!((b.first <= a.second) && (a.first <= b.second))) {
                    return false;
                }
            }

            return true;
            */
        }
    }

    Geometry::CollisionManifold Intersect(Volume &left, Volume &right) {
        return std::visit<Geometry::CollisionManifold>([&](auto &x, auto &y) { return Volume_IntersectImpl(x, y); }, left, right);
    }
} // namespace Elys