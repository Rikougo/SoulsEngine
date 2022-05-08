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
#include <Core/Geometry.hpp>
#include <Core/Logger.hpp>

#include <Render/Camera.hpp>

namespace Elys {
    class TrackBallCamera : public Camera {
      public:
        TrackBallCamera() = default;

        glm::mat4 GetProjection() const override;
        glm::mat4 GetView() const override;
        Frustum GetFrustum() const override;

        void StartCapture() {
            mCapture = true;
            mNewCapture = true;
        }

        enum Direction { UP, DOWN, LEFT, RIGHT, FRONT, BACK };

        void EndCapture() {
            mCapture = false;
            mNewCapture = true;
        }

        void Rotate(float deltaT, float deltaP);
        void Zoom(float delta);
        void Pan(glm::vec3 direction);
        void MouseInput(float x, float y, MouseCode button) override;

        [[nodiscard]] glm::vec3 GetPosition() const override { return mPosition; }
        [[nodiscard]] glm::vec3 GetFront() const { return glm::normalize(mForward); }

      private:
        void UpdateCameraData() const;

      public:
        float speed = 1.0f;
        float sensitivity = 1.0f;
      private:
        // cache data
        mutable Frustum mFrustum;
        mutable glm::mat4 mProjection{1.0f};
        mutable glm::mat4 mView{1.0f};

        glm::vec3 mPosition{0.0f, 0.0f, 0.0f};
        glm::vec3 mForward{0.0, 0.0, 1.0f};
        float mYaw = -90.0f;
        float mPitch = 0.0f;

        bool mCapture = false;
        bool mNewCapture = true;
        float mLastMouseX = 0.0f, mLastMouseY = 0.0f;
    };
} // namespace Elys

#endif // ELYS_TRACKBALL_CAMERA_HPP