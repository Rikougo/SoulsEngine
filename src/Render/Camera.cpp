//
// Created by Sam on 3/14/2022.
//

#include <Render/Camera.hpp>

namespace Elys {
    void Camera::Apply(Shader &shader) const {
        auto projection = glm::perspective(mFOV, mRatioAspect, mNear, mFar);
        auto view = mFree ? glm::lookAt(mPosition, mPosition + mFront, mUp) : glm::lookAt(mPosition, mTarget, mUp);

        shader.SetMat4("projection", projection);
        shader.SetMat4("view", view);
    }

    void Camera::Move(vec2 direction, float speed) {
        if (!mFree) {
            auto cameraDirection = glm::normalize(mTarget - mPosition); // used for zoom
            auto moveRight = glm::cross(mUp, cameraDirection);          // used for side moves

            mPosition += speed * cameraDirection * direction.y; // zoom
            mPosition += speed * moveRight * direction.x;       // side moves
        } else {
            mPosition += speed * mFront * direction.y;
            mPosition += speed * glm::normalize(glm::cross(mUp, mFront)) * direction.x;
        }
    }
}