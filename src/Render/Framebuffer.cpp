//
// Created by sakeiru on 4/1/2022.
//

#include <Render/Framebuffer.hpp>

#include <glad/glad.h>

Elys::Framebuffer::Framebuffer(Elys::FramebufferData data) : mData(data) {
    Update();
}

void Elys::Framebuffer::Resize(float width, float height) {
    mData.Width = width;
    mData.Height = height;

    Update();
}

void Elys::Framebuffer::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, mBufferID);
    glViewport(0, 0, mData.Width, mData.Height);
}

void Elys::Framebuffer::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Elys::Framebuffer::Update() {
    if (mBufferID) {
        glDeleteFramebuffers(1, &mBufferID);
        glDeleteTextures(mColorAttachments.size(), mColorAttachments.data());
        glDeleteTextures(1, &mDepthAttachmentID);

        mColorAttachments.clear();
        mDepthAttachmentID = 0;
    }

    glGenFramebuffers(1, &mBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, mBufferID);

    if (!mData.Attachments.empty()) {
        mColorAttachments.resize(mData.Attachments.size());

        glGenTextures(mColorAttachments.size(), mColorAttachments.data());
        for (size_t i = 0; i < mColorAttachments.size(); i++) {
            glBindTexture(GL_TEXTURE_2D, mColorAttachments[i]);

            switch (mData.Attachments[i].TextureFormat) {
            case GL_RGB:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mData.Width, mData.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
            case GL_RED_INTEGER:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, mData.Width, mData.Height, 0, GL_RED_INTEGER, GL_INT, nullptr);
                break;
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
                                   mColorAttachments[i], 0);
        }
    }

    if (mData.DepthAttachmentFormat != 0) {
        // DEPTH TEXTURE
        glGenTextures(1, &mDepthAttachmentID);
        glBindTexture(GL_TEXTURE_2D, mDepthAttachmentID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, mData.Width, mData.Height, 0,
                     GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D,
                               mDepthAttachmentID, 0);
    }

    glDrawBuffers(mColorAttachments.size(), mColorAttachments.data());

    switch (glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
    case GL_FRAMEBUFFER_UNDEFINED:
        ELYS_CORE_ERROR("Framebuffer::Update : Framebuffer undefined");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        ELYS_CORE_ERROR("Framebuffer::Update : Framebuffer incomplete attachment");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        ELYS_CORE_ERROR("Framebuffer::Update : Framebuffer missing attachment");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        ELYS_CORE_ERROR("Framebuffer::Update : Framebuffer incomplete draw buffer");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        ELYS_CORE_ERROR("Framebuffer::Update : Framebuffer incomplete read buffer");
        break;
    case GL_FRAMEBUFFER_UNSUPPORTED:
        ELYS_CORE_ERROR("Framebuffer::Update : Framebuffer unsupported");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        ELYS_CORE_ERROR("Framebuffer::Update : Framebuffer incomplete multisample");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        ELYS_CORE_ERROR("Framebuffer::Update : Framebuffer incomplete layer targets");
        break;
    case GL_FRAMEBUFFER_COMPLETE:
        ELYS_CORE_INFO("Updated frame successfully [{0}x{1}].", mData.Width, mData.Height);
        break;
    default:
        ELYS_CORE_WARN("Unhandled Framebuffer status.");
    }
}

int Elys::Framebuffer::GetPixel(int x, int y, int layer) {
    Bind();
    int pixelData;
    glReadBuffer(GL_COLOR_ATTACHMENT0 + layer);
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
    Unbind();
    return pixelData;
}
