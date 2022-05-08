//
// Created by Sam on 3/14/2022.
//

#include <Render/TrackBallCamera.hpp>

namespace Elys {
    glm::mat4 TrackBallCamera::GetProjection() const {
        if (mDirty)
            UpdateCameraData();
        return mProjection;
    }
    glm::mat4 TrackBallCamera::GetView() const {
        if (mDirty)
            UpdateCameraData();
        return mView;
    }
    Frustum TrackBallCamera::GetFrustum() const {
        if (mDirty)
            UpdateCameraData();
        return mFrustum;
    }

    void TrackBallCamera::Rotate(float deltaX, float deltaY) {
        mYaw += deltaX;
        mPitch += deltaY;

        if (mPitch > 89.0f)
            mPitch = 89.0f;
        if (mPitch < -89.0f)
            mPitch = -89.0f;

        mForward.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
        mForward.y = sin(glm::radians(mPitch));
        mForward.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));

        mDirty = true;
    }
    void TrackBallCamera::Pan(glm::vec3 direction) {
        mForward = glm::normalize(mForward);
        glm::vec3 right = glm::normalize(glm::cross(mForward, {0.0f, 1.0f, 0.0f}));
        glm::vec3 up = glm::normalize(glm::cross(right, mForward));

        auto deltaSide = right * direction.x * speed;
        auto deltaUP = up * direction.y * speed;
        auto deltaForward = mForward * direction.z * speed;

        mPosition += deltaSide + deltaUP + deltaForward;

        mDirty = true;
    }
    void TrackBallCamera::Zoom(float delta) {
        Pan({0.0f, 0.0f, 1.0f * delta});
    }

    void TrackBallCamera::MouseInput(float x, float y, MouseCode button) {
        if (button == Mouse::ButtonLeft) {
            if (mNewCapture) {
                mLastMouseX = x;
                mLastMouseY = y;
            }
            float xOffset = x - mLastMouseX;
            float yOffset = mLastMouseY - y;
            mLastMouseX = x;
            mLastMouseY = y;

            Rotate(xOffset * sensitivity, yOffset * sensitivity);
        }


        else if (button == Mouse::ButtonRight) {
            if (mNewCapture) {
                mLastMouseX = (x / mViewWidth);
                mLastMouseY = (y / mViewHeight);
            }
            auto dx = mLastMouseX - (x / mViewWidth);
            auto dy = (y / mViewHeight) - mLastMouseY;
            mLastMouseX = (x / mViewWidth);
            mLastMouseY = (y / mViewHeight);

            Pan({dx, 0.0f, dy});
        }
        mNewCapture = false;
    }

    void TrackBallCamera::UpdateCameraData() const {
        auto position = GetPosition();
        auto forward = glm::normalize(mForward);
        auto right =
            glm::normalize(glm::cross(forward, {0.0f, 1.0f, 0.0f})); // cross(front, worldup)
        glm::vec3 up = glm::normalize(glm::cross(right, forward));

        const float halfVSide = mFar * tanf(mFOV * .5f);
        const float halfHSide = halfVSide * mRatioAspect;
        const glm::vec3 frontXFar = mFar * forward;

        // MATRIX
        mProjection = glm::perspective(mFOV, mRatioAspect, mNear, mFar);
        mView = glm::lookAt(position, position + forward, {0.0f, 1.0f, 0.0f});

        auto vp = mProjection * mView;
        glm::vec4 row1 = vp[0], row2 = vp[1], row3 = vp[2], row4 = vp[3];

        mFrustum.nearFace   = Geometry::Plan(position + (mNear * forward), forward);
        mFrustum.farFace    = Geometry::Plan(position + frontXFar, -forward);
        mFrustum.rightFace  = Geometry::Plan(position, glm::cross(up, frontXFar + right * halfHSide));
        mFrustum.leftFace   = Geometry::Plan(position, glm::cross(frontXFar - right * halfHSide, up));
        mFrustum.topFace    = Geometry::Plan(position, glm::cross(right, frontXFar - up * halfVSide));
        mFrustum.bottomFace = Geometry::Plan(position, glm::cross(frontXFar + up * halfVSide, right));

        mDirty = false;
    }
} // namespace Elys