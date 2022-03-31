//
// Created by Sam on 3/29/2022.
//

#ifndef ELYS_CAMERA_HPP
#define ELYS_CAMERA_HPP

#include <glm/glm.hpp>

#include <Core/Geometry.hpp>
#include <Core/KeyCode.hpp>

using glm::vec3;
using glm::mat4;

namespace Elys {
    struct Frustum {
        Geometry::Plan topFace;
        Geometry::Plan bottomFace;

        Geometry::Plan rightFace;
        Geometry::Plan leftFace;

        Geometry::Plan farFace;
        Geometry::Plan nearFace;
    };

    class Camera {
      public:
        virtual ~Camera() = default;

        [[nodiscard]] virtual mat4 GetProjection() const = 0;
        [[nodiscard]] virtual mat4 GetView() const = 0;
        [[nodiscard]] virtual Frustum GetFrustum() const = 0;

        [[nodiscard]] virtual vec3 GetPosition() const = 0;

        virtual void MouseInput(float x, float y, MouseCode button) = 0;

        inline void SetViewSize(float width, float height) {
            mViewWidth = width;
            mViewHeight = height;
            mRatioAspect = width / height;

            mDirty = true;
        }
      protected:
        float mFOV = 45.0f;
        float mViewHeight = 1280.0f, mViewWidth = 720.0f;
        float mRatioAspect = 16.0f / 9.0f;
        float mNear = 0.1f;
        float mFar = 1000.0f;

        mutable bool mDirty = true;
    };
}

#endif // ELYS_CAMERA_HPP
