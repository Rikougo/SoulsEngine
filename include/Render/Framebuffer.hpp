//
// Created by sakeiru on 4/1/2022.
//

#ifndef ELYS_FRAMEBUFFER_HPP
#define ELYS_FRAMEBUFFER_HPP

#include <glm/glm.hpp>

#include <Core/Logger.hpp>

namespace Elys {
    struct FramebufferData {
        float Width;
        float Height;
    };

    class Framebuffer {
      public:
        Framebuffer(FramebufferData data);
        ~Framebuffer() = default;

        [[nodiscard]] unsigned int GetColorTextureID() const { return mColorAttachmentID; }
        [[nodiscard]] FramebufferData GetData() const { return mData; }

        void Bind() const;
        static void Unbind() ;

        void Resize(float width, float height);
      private:
        /// \Brief
        /// When any Framebuffer data (width, height, etc.) changed call this to update Framebuffers/ColorAttachment/DepthAttachment
        void Update();
      private:
        unsigned int mBufferID;
        unsigned int mColorAttachmentID;
        unsigned int mDepthAttachmentID;

        FramebufferData mData;
    };
}

#endif // ELYS_FRAMEBUFFER_HPP
