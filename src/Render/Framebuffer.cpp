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
        glDeleteTextures(1, &mColorAttachmentID);
        glDeleteTextures(1, &mDepthAttachmentID);
        mColorAttachmentID = 0;
        mDepthAttachmentID = 0;
    }
    
    glCreateFramebuffers(1, &mBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, mBufferID);

    // COLOR ATTACHMENT
    glCreateTextures(GL_TEXTURE_2D, 1, &mColorAttachmentID);
    glBindTexture(GL_TEXTURE_2D, mColorAttachmentID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mData.Width, mData.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorAttachmentID, 0);

    // DEPTH TEXTURE
    glCreateTextures(GL_TEXTURE_2D, 1, &mDepthAttachmentID);
    glBindTexture(GL_TEXTURE_2D, mDepthAttachmentID);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, mData.Width, mData.Height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepthAttachmentID, 0);
}
