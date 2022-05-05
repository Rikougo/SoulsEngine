//
// Created by Sam on 3/29/2022.
//

#ifndef ELYS_CAMERA_HPP
#define ELYS_CAMERA_HPP

#include <glm/glm.hpp>

#include <Core/Logger.hpp>
#include <Core/Geometry.hpp>
#include <Core/KeyCode.hpp>

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

        [[nodiscard]] virtual glm::mat4 GetProjection() const = 0;
        [[nodiscard]] virtual glm::mat4 GetView() const = 0;
        [[nodiscard]] virtual Frustum GetFrustum() const = 0;

        [[nodiscard]] virtual glm::vec3 GetPosition() const = 0;

        virtual void MouseInput(float x, float y, MouseCode button) = 0;

        inline void SetViewSize(float width, float height) {
            if (width == 0.0f || height == 0.0f) {
                ELYS_CORE_TRACE("Setting invalid values to camera size [{0}x{1}].", width, height);
                return;
            }

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
