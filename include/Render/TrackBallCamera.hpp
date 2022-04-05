//
// Created by Sam on 3/14/2022.
//

#ifndef ELYS_TRACKBALL_CAMERA_HPP
#define ELYS_TRACKBALL_CAMERA_HPP

#include <cmath>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Core/Base.hpp>
#include <Core/Logger.hpp>
#include <Core/Geometry.hpp>

#include <Render/Camera.hpp>

using glm::vec2;
using glm::vec3;
using glm::mat4;

namespace Elys {
    class TrackBallCamera : public Camera {
      public:
        TrackBallCamera() = default;

        mat4 GetProjection() const override;
        mat4 GetView() const override;
        Frustum GetFrustum() const override;

        void StartCapture(int button) {
            mCapture = true;
            mNewCapture = true;
            mButtonType = button;
        }

        void EndCapture() { mCapture = false; mNewCapture = true; }

        void Rotate(float deltaT, float deltaP);
        void Translate(vec3 direction, float speed=1.0f);
        void Zoom(float delta);
        void Pan(float deltaX, float deltaY);
        void MouseInput(float x, float y, MouseCode button) override;

        [[nodiscard]] vec3 GetPosition() const override { return mPosition + Geometry::ToCartesian(mPhi, mTheta, mDistance); }
        [[nodiscard]] vec2 GetRotation() const { return {mPhi, mTheta}; }
        [[nodiscard]] float GetUp() const { return mUp; }
        [[nodiscard]] float GetDistance() const { return mDistance; }
        [[nodiscard]] vec3 GetTarget() const { return mTarget; }

      private:
        void UpdateCameraData() const;

      private:
        // cache data
        mutable Frustum mFrustum;
        mutable mat4 mProjection{1.0f};
        mutable mat4 mView{1.0f};

        vec3 mPosition{0.0f, 0.0f, 0.0f};
        vec3 mDirection;
        float mYaw = -90.0f;
        float mPitch = 0.0f;

        float mTheta = 0.0f, mPhi = (float)M_PI / 2;
        float mUp = 1.0f;
        float mDistance = 5.0f;
        vec3 mTarget{0.0f, 0.0f, 0.0f};

        bool mCapture = false;
        bool mNewCapture = true;
        int mButtonType = -1;
        float mLastMouseX = 0.0f, mLastMouseY = 0.0f;
        float mZoomSpeed = 1.0f;
    };
}

#endif // ELYS_TRACKBALL_CAMERA_HPP
