//
// Created by pierrhum on 08/05/2022.
//
#include "Render/BoundingBox.hpp"

using glm::vec3;

void Elys::BoundingBox::RenderUpdate() {
    if (mVAO) {
        UpdateVertices();

        mVAO->GetVertexBuffer()->SetData((void *)mVertices.data(),
                                         static_cast<uint32_t>(mVertices.size() * sizeof(vec3)),
                                         GL_DYNAMIC_DRAW);
    } else {
        InitBuffers();
    }
}
void Elys::BoundingBox::InitBuffers() {
    UpdateVertices();

    mVAO.reset();
    mVAO = std::make_shared<VertexArray>();
    auto vbo = std::make_shared<VertexBuffer>(
        (void *)mVertices.data(), static_cast<uint32_t>(mVertices.size() * sizeof(vec3)),
        GL_DYNAMIC_DRAW);
    BufferLayout vertexLayout{{"position", sizeof(vec3), 3, GL_FLOAT}};
    vbo->SetLayout(vertexLayout);
    mVAO->SetVertexBuffer(vbo);

}
