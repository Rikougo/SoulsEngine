//
// Created by pierrhum on 08/05/2022.
//

#include "Physics/OBB.hpp"

namespace Elys {
    OBB::OBB(glm::vec3 const &center, glm::vec3 const &s, glm::mat3 const &rotation) :
        mCenter(center), mSize(s), mRotation(rotation) {
        UpdateVertices();
    }

    void OBB::Update(glm::vec3 const &center, glm::mat3 const &rotation) {
        mCenter = center;
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

    void OBB::UpdateBuffers() {
        // VERTEX ARRAY ALREADY EXIST THEN UPDATE ONLY VERTEX BUFFER DATA
        if (mVAO) {
            mVAO->GetVertexBuffer()->SetData((void *)mVertices.data(),
                                             static_cast<uint32_t>(mVertices.size() * sizeof(glm::vec3)),
                                             GL_DYNAMIC_DRAW);
        }
        // INIT VERTEX ARRAY
        else {
            // LINE INDICES
            static uint32_t indices[] = {
                6, 1,
                6, 3,
                6, 4,
                2, 7,
                2, 5,
                2, 0,
                0, 1,
                0, 3,
                7, 1,
                7, 4,
                4, 5,
                5, 3
            };

            mVAO = std::make_shared<VertexArray>();
            auto vbo = std::make_shared<VertexBuffer>(
                (void *)mVertices.data(), static_cast<uint32_t>(mVertices.size() * sizeof(glm::vec3)),
                GL_DYNAMIC_DRAW);

            auto ebo = std::make_shared<IndexBuffer>(indices, 24);
            BufferLayout vertexLayout{{"position", sizeof(glm::vec3), 3, GL_FLOAT}};
            vbo->SetLayout(vertexLayout);
            mVAO->SetVertexBuffer(vbo);
            mVAO->SetIndexBuffer(ebo);
        }
    }

    std::pair<float, float> OBB::GetInterval(glm::vec3 const &axis) {
        if (mVertices.empty()) return {0, 0};

        std::pair<float, float> result{
            glm::dot(axis, mVertices[0]),
            glm::dot(axis, mVertices[0])
        };

        for(uint8_t i = 1; i < 8; i++) {
            float projection = glm::dot(axis, mVertices[i]);

            result.first = projection < result.first ? projection : result.first;
            result.second = projection > result.second ? projection : result.second;
        }

        return result;
    }
}
