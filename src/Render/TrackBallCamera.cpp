//
// Created by Sam on 3/14/2022.
//

#include <Render/TrackBallCamera.hpp>

namespace Elys {
    mat4 TrackBallCamera::GetProjection() const {
        if (mDirty) UpdateCameraData();
        return mProjection;
    }
    mat4 TrackBallCamera::GetView() const {
        if (mDirty) UpdateCameraData();
        return mView;
    }
    Frustum TrackBallCamera::GetFrustum() const {
        if (mDirty) UpdateCameraData();

        return mFrustum;
    }

    void TrackBallCamera::Rotate(float deltaT, float deltaP) {
        if (mUp > 0.0f) {
            mTheta += deltaT;
        } else {
            mTheta -= deltaT;
        }

        mPhi += deltaP;

        // Keep phi within -2PI to +2PI for easy 'up' comparison
        if (mPhi > M_2_PI) {
            mPhi -= M_2_PI;
        } else if (mPhi < -M_2_PI) {
            mPhi += M_2_PI;
        }

        // If phi is between 0 to PI or -PI to -2PI, make 'up' be positive Y, other wise make it
        // negative Y
        if ((mPhi > 0 && mPhi < M_PI) || (mPhi < -M_PI && mPhi > -M_2_PI)) {
            mUp = 1.0f;
        } else {
            mUp = -1.0f;
        }

        mDirty = true;
    }
    void TrackBallCamera::Pan(float deltaX, float deltaY) {
        vec3 look = normalize(-Geometry::ToCartesian(mPhi, mTheta, mDistance));
        vec3 worldUp = {0.0f, mUp, 0.0f};

        vec3 right = normalize(glm::cross(look, worldUp));
        vec3 up = normalize(glm::cross(look, right));

        mTarget += (right * deltaX + up * deltaY);

        mDirty = true;
    }
    void TrackBallCamera::Zoom(float delta) {
        mDistance -= delta * mZoomSpeed;

        if (mDistance < 1.0f) {
            mDistance = 1.0f;
        }

        mDirty = true;
    }

    void TrackBallCamera::MouseInput(float x, float y, MouseCode button)    {
        if (mNewCapture) {
            mLastMouseX = x / mViewWidth;
            mLastMouseY = y / mViewHeight;
        }

        mNewCapture = false;

        auto dx = mLastMouseX - (x / mViewWidth);
        auto dy = mLastMouseY - (y / mViewHeight);

        mLastMouseX = (x / mViewWidth);
        mLastMouseY = (y / mViewHeight);

        // DEAD ZONE
        dx = (abs(dx) < 0.001f) ? 0.0f : dx;
        dy = (abs(dy) < 0.001f) ? 0.0f : dy;

        if (dx == 0.0f && dy == 0.0f) return;

        if (button == Mouse::ButtonLeft) {
            Rotate(-dx * static_cast<float>(M_2_PI), dy * static_cast<float>(M_2_PI));
        } else if (button == Mouse::ButtonRight) {
            Pan(dx * 10.0f, dy * 10.0f);
        }
    }

    void TrackBallCamera::UpdateCameraData() const {
        {
            // FRUSTUM
            auto position = GetPosition();
            auto front = glm::normalize(mTarget - position);
            auto right = glm::normalize(glm::cross(front, {0.0f, 1.0f, 0.0f})); // cross(front, worldup)
            vec3 up = glm::normalize(glm::cross(right, front));

            const float halfVSide = mFar * tanf(mFOV * .5f);
            const float halfHSide = halfVSide * mRatioAspect;
            const glm::vec3 frontMultFar = mFar * front;

            // MATRIX
            mProjection = glm::perspective(mFOV, mRatioAspect, mNear, mFar);
            mView = glm::lookAt(position, mTarget, {0.0f, mUp, 0.0f});

            auto vp = mProjection * mView;
            glm::vec4 row1 = vp[0], row2 = vp[1], row3 = vp[2], row4 = vp[3];


            mFrustum.nearFace   = Geometry::Plan(position + (mNear * front), front);
            mFrustum.farFace    = Geometry::Plan(position + frontMultFar, -front);
            mFrustum.rightFace  = Geometry::Plan(position, glm::cross(up, frontMultFar + right * halfHSide));
            mFrustum.leftFace   = Geometry::Plan(position, glm::cross(frontMultFar - right * halfHSide, up));
            mFrustum.topFace    = Geometry::Plan(position, glm::cross(right, frontMultFar - up * halfVSide));
            mFrustum.bottomFace = Geometry::Plan(position, glm::cross(frontMultFar + up * halfVSide, right));


            mDirty = false;
        }
    }
} // namespace Elys