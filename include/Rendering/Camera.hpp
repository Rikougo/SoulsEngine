//
// Created by Sam on 1/30/2022.
//

#ifndef ELYS_CAMERA_HPP
#define ELYS_CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Rendering/Shader.hpp"

using glm::vec2;
using glm::vec3;
using glm::mat4;

namespace Core::Rendering {
class Camera {
  public:
    vec3 mPosition{0.0f, 2.0f, -5.0f};
    vec3 mTarget{0.0f, 0.0f, 0.0f};
    vec3 mUp{0.0f, 1.0f, 0.0f};
    vec3 mFront{1.0f, 0.0f, 0.0f};

    float mFOV = 45.0f;
    float mRatioAspect = 16.0f / 9.0f;
    float mNear = 0.1f;
    float mFar = 1000.0f;

    bool mFree = false;

    Camera() = default;

    void apply(Shader &shader) const;

    void move(vec2 direction, float speed);

    void SetAspect(float ratio_aspect) { mRatioAspect = ratio_aspect; }
    void SetFree(bool isFree) { mFree = isFree; }
    void SetFront(vec3 front) { mFront = front; }
};
}

#endif // ELYS_CAMERA_HPP
