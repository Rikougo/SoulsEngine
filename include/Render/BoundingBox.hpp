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
        [[nodiscard]] virtual std::vector<vec3> Vertices() const = 0;
        [[nodiscard]] virtual bool IsInFrustum(Frustum frustum, mat4 transform) const = 0;
        [[nodiscard]] void Draw() const;

        [[nodiscard]] unsigned int VAO() const { return mVAO; }
        [[nodiscard]] bool ShouldDraw() const { return mShouldDraw; }

      protected:
        void GenerateBuffers();
      private:
        unsigned int mVAO, mVBO, mEBO;
        bool mShouldDraw = true;
    };
}

#endif // ELYS_BOUNDING_BOX_HPP
