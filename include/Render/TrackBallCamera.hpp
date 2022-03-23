//
// Created by Sam on 3/14/2022.
//

#ifndef ELYS_TRACKBALLCAMERA_HPP
#define ELYS_TRACKBALLCAMERA_HPP

#include <cmath>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Render/Shader.hpp>

using glm::vec2;
using glm::vec3;
using glm::mat4;

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#ifndef M_2_PI
    #define M_2_PI 2 * M_PI
#endif

namespace Elys {
    class TrackBallCamera {
      public:
        TrackBallCamera() = default;

        void Apply(Shader &shader) const;

        inline void SetViewSize(float width, float height) {
            mViewWidth = width;
            mViewHeight = height;
            mRatioAspect = width / height;
        }

        void StartCapture(int button) {
            mCapture = true;
            mNewCapture = true;
            mButtonType = button;
        }

        void EndCapture() { mCapture = false; }

        void Rotate(float deltaT, float deltaP);
        void Zoom(float delta);
        void Pan(float deltaX, float deltaY);
        void MouseInput(float x, float y);

        vec2 GetRotation() { return {mPhi, mTheta}; }
        float GetUp() { return mUp; }
        float GetDistance() { return mDistance; }
        vec3 GetPosition() { return mTarget + ToCartesian(mPhi, mTheta, mDistance); }
        vec3 GetTarget() { return mTarget; }

      private:
        static vec3 ToCartesian(float phi, float theta, float radius) {
            float x = radius * sin(phi) * sin(theta);
            float y = radius * cos(phi);
            float z = radius * sin(phi) * cos(theta);

            return {x, y, z};
        }

      private:
        float mTheta = 0.1f, mPhi = 0.1f;
        float mUp = 1.0f;
        float mDistance = 5.0f;
        vec3 mTarget{0.0f, 0.0f, 0.0f};

        float mFOV = 45.0f;
        float mViewHeight = 1280.0f, mViewWidth = 720.0f;
        float mRatioAspect = 16.0f / 9.0f;
        float mNear = 0.1f;
        float mFar = 1000.0f;

        bool mCapture = false;
        bool mNewCapture = true;
        int mButtonType;
        float mLastMouseX = 0.0f, mLastMouseY = 0.0f;
    };
}

#endif // ELYS_TRACKBALLCAMERA_HPP
