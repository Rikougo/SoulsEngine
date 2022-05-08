//
// Created by pierrhum on 08/05/2022.
//

#include <Render/OBB.hpp>

namespace Elys {
    OBB::OBB(glm::vec3 const &center, glm::vec3 const &size, glm::mat3 const &rotation) :
        mCenter(center), mSize(size), mRotation(rotation) {
    }

    void OBB::Update(glm::vec3 const &center, glm::vec3 const &size, glm::mat3 const &rotation) {
        mCenter = center;
        mSize = size;
        mRotation = rotation;

        UpdateVertices();
    }

    void OBB::UpdateVertices() {
        mVertices = {
            mCenter + mRotation[0] * mSize[0] + mRotation[1] * mSize[1] + mRotation[2] * mSize[2],
            mCenter - mRotation[0] * mSize[0] + mRotation[1] * mSize[1] + mRotation[2] * mSize[2],
            mCenter + mRotation[0] * mSize[0] - mRotation[1] * mSize[1] + mRotation[2] * mSize[2],
            mCenter + mRotation[0] * mSize[0] + mRotation[1] * mSize[1] - mRotation[2] * mSize[2],
            mCenter - mRotation[0] * mSize[0] - mRotation[1] * mSize[1] - mRotation[2] * mSize[2],
            mCenter + mRotation[0] * mSize[0] - mRotation[1] * mSize[1] - mRotation[2] * mSize[2],
            mCenter - mRotation[0] * mSize[0] + mRotation[1] * mSize[1] - mRotation[2] * mSize[2],
            mCenter - mRotation[0] * mSize[0] - mRotation[1] * mSize[1] + mRotation[2] * mSize[2],
        };
    }

    std::pair<float, float> OBB::GetInterval(OBB const &obb, glm::vec3 const &axis) {
        std::pair<float, float> result{
            glm::dot(axis, obb.mVertices[0]),
            glm::dot(axis, obb.mVertices[0])
        };

        for(uint8_t i = 1; i < 8; i++) {
            float projection = glm::dot(axis, obb.mVertices[i]);

            result.first = projection < result.first ? projection : result.first;
            result.second = projection > result.second ? projection : result.second;
        }

        return result;
    }

    bool OBB::CollapseOnAxis(const OBB &left, const OBB &right, const glm::vec3 &axis) {
        auto a = OBB::GetInterval(left, axis);
        auto b = OBB::GetInterval(right, axis);

        return ((b.first <= a.second) && (a.first <= b.second));
    }

    bool OBB::Collapse(const OBB &left, const OBB &right) {
        std::array<glm::vec3, 15> testAxis = {
            left.mRotation[0], left.mRotation[1], left.mRotation[2],
            right.mRotation[0], right.mRotation[1], right.mRotation[2]
        };

        for(uint8_t i = 0; i < 3; i++) {
            testAxis[6 + i * 3 + 0] = glm::cross(testAxis[i], testAxis[3]);
            testAxis[6 + i * 3 + 1] = glm::cross(testAxis[i], testAxis[4]);
            testAxis[6 + i * 3 + 2] = glm::cross(testAxis[i], testAxis[5]);
        }

        for(auto const &axis : testAxis) {
            if (!CollapseOnAxis(left, right, axis)) return false;
        }

        return true;
    }
}
