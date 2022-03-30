//
// Created by Sam on 3/29/2022.
//

#ifndef ELYS_BOUNDING_BOX_HPP
#define ELYS_BOUNDING_BOX_HPP

#include <glm/glm.hpp>

#include <Render/Camera.hpp>

using glm::mat4;

namespace Elys {
    class BoundingBox {
      public:
        virtual bool IsInFrustum(Frustum frustum, mat4 transform) const = 0;
    };
}

#endif // ELYS_BOUNDING_BOX_HPP
