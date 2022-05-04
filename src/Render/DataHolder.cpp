//
// Created by Sam on 4/8/2022.
//

#include <Render/DataHolder.hpp>

namespace Elys {
    VertexBuffer::VertexBuffer(const void *data, uint32_t count, GLenum usage) {
        glGenBuffers(1, &mID);
        glBindBuffer(GL_ARRAY_BUFFER, mID);
        glBufferData(GL_ARRAY_BUFFER, count, data, usage);
    }
    VertexBuffer::~VertexBuffer() {
        glDeleteBuffers(1, &mID);
    }
    void VertexBuffer::Bind() {
        glBindBuffer(GL_ARRAY_BUFFER, mID);
    }
    void VertexBuffer::Unbind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    void VertexBuffer::SetData(const void *data, uint32_t count, GLenum usage) {
        glDeleteBuffers(1, &mID);

        glGenBuffers(1, &mID);
        glBindBuffer(GL_ARRAY_BUFFER, mID);
        glBufferData(GL_ARRAY_BUFFER, count, data, usage);
    }

    IndexBuffer::IndexBuffer(uint32_t *indices, uint32_t size) : mSize(size) {
        glGenBuffers(1, &mID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(uint32_t), indices, GL_STATIC_DRAW);
    }
    IndexBuffer::~IndexBuffer() {
        glDeleteBuffers(1, &mID);
    }
    void IndexBuffer::Bind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
    }
    void IndexBuffer::Unbind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    uint32_t IndexBuffer::Size() { return mSize; }

    VertexArray::VertexArray() : mVertexIndex(0) {
        glGenVertexArrays(1, &mID);
    }
    VertexArray::~VertexArray() {
        glDeleteVertexArrays(1, &mID);
    }
    void VertexArray::Bind() const { glBindVertexArray(mID); }
    void VertexArray::Unbind() const { glBindVertexArray(0); }

    void VertexArray::SetVertexBuffer(std::shared_ptr<VertexBuffer> &buffer) {
        Bind();
        buffer->Bind();

        const auto& layout = buffer->GetLayout();
        mVertexBuffer = buffer;
        for(const auto &element : layout) {
            glEnableVertexAttribArray(mVertexIndex);
            glVertexAttribPointer(
                mVertexIndex,
                element.size,
                element.glType,
                element.normalize ? GL_TRUE : GL_FALSE,
                layout.GetStride(),
                (void*)element.offset
            );
            mVertexIndex++;
        }
    }

    void VertexArray::SetIndexBuffer(std::shared_ptr<IndexBuffer> &buffer) {
        Bind();
        buffer->Bind();

        mIndexBuffer = buffer;
    }

}