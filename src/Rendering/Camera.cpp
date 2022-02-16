//
// Created by Sam on 1/30/2022.
//

#include "Rendering/Camera.hpp"

using namespace Core::Rendering;

void Camera::apply(Shader &shader) const {
    auto projection = glm::perspective(mFOV, mRatioAspect, mNear, mFar);
    auto view = mFree ? glm::lookAt(mPosition, mPosition + mFront, mUp) : glm::lookAt(mPosition, mTarget, mUp);

    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
}

void Camera::move(vec2 direction, float speed) {
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
