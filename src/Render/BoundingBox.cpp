//
// Created by pierrhum on 21/04/2022.
//

#include <Render/BoundingBox.hpp>
#include <glad/glad.h>

void Elys::BoundingBox::GenerateBuffers() {
    std::vector<vec3> vertices = Vertices();

    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    glBufferData(
        GL_ARRAY_BUFFER,                                             // BUFFER TYPE
        static_cast<GLsizeiptr>(vertices.size() * sizeof(vec3)),    // BUFFER SIZE
        &vertices[0],                                               // BUFFER ADDRESS
        GL_STATIC_DRAW                                               // USAGE
    );

    // SET UP VERTEX ARRAY LAYOUT
    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);

    // unbind VAO
    glBindVertexArray(0);


}
void Elys::BoundingBox::Draw() const {
    glBindVertexArray(mVAO);
    glDrawElements(GL_LINE_STRIP, Vertices().size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
