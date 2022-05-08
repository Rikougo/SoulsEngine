//
// Created by pierrhum on 08/05/2022.
//
#include "Physics/BoundingBox.hpp"

using glm::vec3;

namespace Elys {
    // Update buffers impl
    namespace {
        void Volume_UpdateBuffersImpl(AABB &aabb) { ELYS_CORE_INFO("AABB call"); }

        void Volume_UpdateBuffersImpl(OBB &obb) { ELYS_CORE_INFO("OBB call"); }
    } // namespace

    void UpdateBuffers(Volume &volume) {
        std::visit([&](auto &x) { Volume_UpdateBuffersImpl(x); }, volume);
    }

    // Intersect impl
    namespace {
        bool Volume_IntersectImpl(AABB &left, AABB &right) {
            auto [leftLo, leftHi] = left.GetBounds();
            auto [rightLo, rightHi] = right.GetBounds();

            return (leftLo.x <= rightHi.x && leftHi.x >= rightLo.x) &&
               (leftLo.y <= rightHi.y && leftHi.y >= rightLo.y) &&
               (leftLo.z <= rightHi.z && leftHi.z >= rightLo.z);
        }

        bool Volume_IntersectImpl(AABB &left, OBB &right) {
            return false;
        }

        bool Volume_IntersectImpl(OBB &left, AABB &right) {
            return Volume_IntersectImpl(right, left);
        }

        bool Volume_IntersectImpl(OBB &left, OBB &right) {
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
                auto a = OBB::GetInterval(left, axis);
                auto b = OBB::GetInterval(right, axis);

                if (!((b.first <= a.second) && (a.first <= b.second))) {
                    return false;
                }
            }

            return true;
        }
    }

    bool Intersect(Volume &left, Volume &right) {
        return std::visit<bool>([&](auto &x, auto &y) { return Volume_IntersectImpl(x, y); }, left, right);
    }
} // namespace Elys