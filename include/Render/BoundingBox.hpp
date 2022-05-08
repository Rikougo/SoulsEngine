//
// Created by pierrhum on 08/05/2022.
//

#ifndef ELYS_BOUNDINGBOX_HPP
#define ELYS_BOUNDINGBOX_HPP

#include <glm/glm.hpp>
#include "Mesh.hpp"

namespace Elys {
    class BoundingBox {
      public:
        BoundingBox(){};
        virtual void Update(glm::mat4 transform, Mesh const &mesh){};
        void RenderUpdate();
        void SetCollided(bool value) { mCollided = value; }

        [[nodiscard]] std::shared_ptr<VertexArray> VAO() const { return mVAO; }
        [[nodiscard]] std::vector<glm::vec3> Vertices() const { return mVertices; }
        [[nodiscard]] bool IsCollided() const { return mCollided; }

      protected:
        void InitBuffers();
        virtual void UpdateVertices() {};

        mutable std::shared_ptr<VertexArray> mVAO;
        bool mCollided = false;
        std::vector<glm::vec3> mVertices;
        glm::mat4 mTransform{}; // transform cache
    };
}
#endif // ELYS_BOUNDINGBOX_HPP
