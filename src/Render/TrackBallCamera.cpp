//
// Created by Sam on 3/14/2022.
//

#include <Render/TrackBallCamera.hpp>

namespace Elys {
    void TrackBallCamera::Apply(Shader &shader) const {
        auto position = mTarget + ToCartesian(mPhi, mTheta, mDistance);

        auto projection = glm::perspective(mFOV, mRatioAspect, mNear, mFar);
        auto view = glm::lookAt(position, mTarget, {0.0f, mUp, 0.0f});

        shader.SetMat4("projection", projection);
        shader.SetMat4("view", view);
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
    }

    void TrackBallCamera::Pan(float deltaX, float deltaY) {
        vec3 look = normalize(-ToCartesian(mPhi, mTheta, mDistance));
        vec3 worldUp = {0.0f, mUp, 0.0f};

        vec3 right = normalize(glm::cross(look, worldUp));
        vec3 up = normalize(glm::cross(look, right));

        mTarget += (right * deltaX + up * deltaY);
    }

    void TrackBallCamera::Zoom(float delta) {
        mDistance -= delta;

        if (mDistance < 1.0f) {
            mDistance = 1.0f;
        }
    }

    void TrackBallCamera::MouseInput(float x, float y) {
        if (!mCapture)
            return;

        if (mNewCapture) {
            mLastMouseX = x / mViewWidth;
            mLastMouseY = y / mViewHeight;
        }

        mNewCapture = false;

        auto dx = mLastMouseX - (x / mViewWidth);
        auto dy = mLastMouseY - (y / mViewHeight);

        mLastMouseX = (x / mViewWidth);
        mLastMouseY = (y / mViewHeight);

        if (mButtonType == 0) {
            Rotate(-dx * M_2_PI, dy * M_2_PI);
        } else if (mButtonType == 1) {
            Pan(dx * 10.0f, dy * 10.0f);
        }
    }
} // namespace Elys