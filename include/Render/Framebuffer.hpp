//
// Created by sakeiru on 4/1/2022.
//

#ifndef ELYS_FRAMEBUFFER_HPP
#define ELYS_FRAMEBUFFER_HPP

#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <Core/Logger.hpp>

namespace Elys {
    struct FramebufferAttachmentSpec {
        GLenum TextureFormat;
    };

    struct FramebufferData {
        float Width;
        float Height;
        int Sample = 1;

        std::vector<FramebufferAttachmentSpec> Attachments;
        GLenum DepthAttachmentFormat = 0;
    };

    class Framebuffer {
      public:
        Framebuffer(FramebufferData data);
        ~Framebuffer() = default;

        [[nodiscard]] unsigned int GetColorTextureID(int layer = 0) const { return mColorAttachments[layer]; }
        [[nodiscard]] FramebufferData GetData() const { return mData; }

        [[nodiscard]] int GetPixel(int x, int y, int layer = 0);

        void Bind() const;
        static void Unbind() ;

        void Resize(float width, float height);
      private:
        /// \Brief
        /// When any Framebuffer data (width, height, etc.) changed call this to update Framebuffers/ColorAttachment/DepthAttachment
        void Update();
      private:
        unsigned int mBufferID;
        std::vector<unsigned int> mColorAttachments;
        unsigned int mEntityAttachmentID;
        unsigned int mDepthAttachmentID;

        FramebufferData mData;
    };
}

#endif // ELYS_FRAMEBUFFER_HPP
