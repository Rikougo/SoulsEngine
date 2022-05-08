//
// Created by Sam on 3/14/2022.
//

#ifndef ELYS_TRACKBALL_CAMERA_HPP
#define ELYS_TRACKBALL_CAMERA_HPP

#include <cmath>
#include <iostream>
#include <numbers>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Core/Base.hpp>
#include <Core/Logger.hpp>
#include <Core/Geometry.hpp>

#include <Render/Camera.hpp>

namespace Elys {
    class TrackBallCamera : public Camera {
      public:
        TrackBallCamera() = default;

        glm::mat4 GetProjection() const override;
        glm::mat4 GetView() const override;
        Frustum GetFrustum() const override;

        void StartCapture(int button) {
            mCapture = true;
            mNewCapture = true;
            mButtonType = button;
        }

        void EndCapture() { mCapture = false; mNewCapture = true; }

        void Rotate(float deltaT, float deltaP);
        void Zoom(float delta);
        void Pan(float deltaX, float deltaY);
        void MouseInput(float x, float y, MouseCode button) override;
        void SetTarget(glm::vec3 newTarget) { mTarget = newTarget; mDirty = true; }

        [[nodiscard]] glm::vec3 GetPosition() const override { return mTarget + Geometry::ToCartesian(mPhi, mTheta, mDistance); }
        [[nodiscard]] glm::vec2 GetRotation() const { return {mPhi, mTheta}; }
        [[nodiscard]] float GetUp() const { return mUp; }
        [[nodiscard]] float GetDistance() const { return mDistance; }
        [[nodiscard]] glm::vec3 GetTarget() const { return mTarget; }

      private:
        void UpdateCameraData() const;

      private:
        // cache data
        mutable Frustum mFrustum;
        mutable glm::mat4 mProjection{1.0f};
        mutable glm::mat4 mView{1.0f};

        float mTheta = std::numbers::pi_v<float>, mPhi = std::numbers::pi_v<float> / 2.0f;
        float mUp = 1.0f;
        float mDistance = 5.0f;
        glm::vec3 mTarget{0.0f, 0.0f, 0.0f};

        bool mCapture = false;
        bool mNewCapture = true;
        int mButtonType = -1;
        float mLastMouseX = 0.0f, mLastMouseY = 0.0f;
        float mZoomSpeed = 1.0f;
    };
}

#endif // ELYS_TRACKBALL_CAMERA_HPP
