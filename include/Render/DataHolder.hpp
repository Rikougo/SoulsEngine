//
// Created by Sam on 4/8/2022.
//

#ifndef ELYS_DATA_HOLDER_HPP
#define ELYS_DATA_HOLDER_HPP

#include <string>
#include <vector>
#include <memory>

#include <glad/glad.h>

namespace Elys {
    /**
     * @brief
     * Give details about Buffer element. Name for debug purpose, size to compute offset. Offset is set
     * while used in BufferLayout (see BufferLayout::ComputeBufferElement)
     */
    struct BufferElement {
        std::string name;
        uint32_t offset;
        uint32_t size;
        uint32_t dataSize;
        bool normalize;
        GLenum glType;

        BufferElement() = default;

        BufferElement(std::string name, uint32_t dataSize, uint32_t size, GLenum type, bool normalize = false) :
            name(std::move(name)), dataSize(dataSize), size(size), glType(type), normalize(normalize) {}
    };

    class BufferLayout {
      private:
        uint32_t mStride;
        std::vector<BufferElement> mElements;
      public: /** ITERATORS */
        std::vector<BufferElement>::iterator begin() { return mElements.begin(); }
        std::vector<BufferElement>::iterator end() { return mElements.end(); }
        [[nodiscard]] std::vector<BufferElement>::const_iterator begin() const { return mElements.begin(); }
        [[nodiscard]] std::vector<BufferElement>::const_iterator end() const { return mElements.end(); }
      public:
        BufferLayout() = default;
        BufferLayout(std::initializer_list<BufferElement> elements) : mElements(elements) {
            ComputeBufferElement();
        }

        [[nodiscard]] uint32_t GetStride() const { return mStride; }
        [[nodiscard]] size_t ElementAmount() const { return mElements.size(); }
      private:
        /**
         * @brief
         * Iterate through all elements stored in layout to compute offset and stride
         */
        void ComputeBufferElement() {
            mStride = 0;
            for(auto &element : mElements) {
                element.offset = mStride;
                mStride += element.dataSize;
            }
        }
    };

    class VertexBuffer {
      public:
        /**
         * @brief Create buffer with given data
         * @param data data contained in buffer
         * @param count amount of data (in bytes)
         */
        VertexBuffer(const void* data, uint32_t count, GLenum usage = GL_STATIC_DRAW);
        ~VertexBuffer();

        void Bind();
        void Unbind();

        /**
         * @brief Destroy and recreate buffer with new data
         * @param data data contained in buffer
         * @param count amount of data (in bytes)
         */
        void SetData(const void* data, uint32_t count, GLenum usage = GL_STATIC_DRAW);
        void SetLayout(const BufferLayout &bufferLayout) { mLayout = bufferLayout; }
        [[nodiscard]] const BufferLayout& GetLayout() const { return mLayout; }
      private:
        uint32_t mID;
        BufferLayout mLayout;
    };

    class IndexBuffer {
      public:
        IndexBuffer(uint32_t *indices, uint32_t size);
        ~IndexBuffer();

        void Bind();
        void Unbind();

        uint32_t Size();
      private:
        uint32_t mID;
        uint32_t mSize;
    };

    class VertexArray {
      public:
        VertexArray();
        ~VertexArray();

        void Bind() const;
        void Unbind() const;

        void SetVertexBuffer(std::shared_ptr<VertexBuffer> &buffer);
        void SetIndexBuffer(std::shared_ptr<IndexBuffer> &buffer);

        [[nodiscard]] std::shared_ptr<VertexBuffer>& GetVertexBuffer() { return mVertexBuffer; }
        [[nodiscard]] std::shared_ptr<IndexBuffer>& GetIndexBuffer() { return mIndexBuffer; }
      private:
        uint32_t mID;
        uint32_t mVertexIndex = 0;

        std::shared_ptr<VertexBuffer> mVertexBuffer;
        std::shared_ptr<IndexBuffer> mIndexBuffer;
    };
}


#endif // ELYS_DATA_HOLDER_HPP
