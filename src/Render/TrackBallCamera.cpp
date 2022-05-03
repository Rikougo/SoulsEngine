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

    void TrackBallCamera::Rotate(float deltaX, float deltaY) {
        mYaw   += deltaX;
        mPitch += deltaY;

        if(mPitch > 89.0f)
            mPitch = 89.0f;
        if(mPitch < -89.0f)
            mPitch = -89.0f;

        mDirection.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
        mDirection.y = sin(glm::radians(mPitch));
        mDirection.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
        mTarget = glm::normalize(mDirection);

        mDirty = true;
    }
    void TrackBallCamera::Pan(float deltaX, float deltaY) {
        Translate(RIGHT, deltaX);
        Translate(UP, deltaY);

        mDirty = true;
    }
    void TrackBallCamera::Zoom(float delta) {
        mPosition += mTarget * delta * mZoomSpeed;

        mDirty = true;
    }

    void TrackBallCamera::MouseInput(float x, float y, MouseCode button)    {
        float sensitivity = 0.1f;

        /// Rotation de la caméra
        if (button == Mouse::ButtonLeft) {
            if (mNewCapture) {
                mLastMouseX = x;
                mLastMouseY = y;
            }
            float xoffset = x - mLastMouseX;
            float yoffset = mLastMouseY - y;
            mLastMouseX = x;
            mLastMouseY = y;

            Rotate(xoffset * sensitivity, yoffset * sensitivity);
        }
        /// Déplacement de la caméra
        else if (button == Mouse::ButtonRight) {
            if (mNewCapture) {
                mLastMouseX = (x / mViewWidth);
                mLastMouseY = (y / mViewHeight);
            }
            auto dx = mLastMouseX - (x / mViewWidth);
            auto dy = (y / mViewHeight) - mLastMouseY;
            mLastMouseX = (x / mViewWidth);
            mLastMouseY = (y / mViewHeight);

            Pan(dx * 10.0f, dy *  10.0f);
        }
        mNewCapture = false;


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
            mView = glm::lookAt(position, position + mTarget, {0.0f, mUp, 0.0f});

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
    void TrackBallCamera::Translate(Direction direction, float speed) {
        vec3 up, right;
        auto position = GetPosition();
        auto front = glm::normalize(mTarget - position);
        right = glm::normalize(glm::cross(front, {0.0f, 1.0f, 0.0f}));
        up = glm::normalize(glm::cross(right, front));

        switch(direction)
        {
        case UP:
            mPosition += up * speed * mZoomSpeed;
            break;
        case DOWN:
            mPosition -= up * speed * mZoomSpeed;
            break;
        case LEFT:
            mPosition -= right * speed * mZoomSpeed;
            break;
        case RIGHT:
            mPosition += right * speed * mZoomSpeed;
            break;
        case FRONT:
            mPosition += mTarget * speed * mZoomSpeed;
            break;
        case BACK:
            mPosition -= mTarget * speed * mZoomSpeed;
            break;
        }

        mDirty = true;
    }
} // namespace Elys