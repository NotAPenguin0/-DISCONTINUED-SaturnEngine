#include "FrameBuffer.hpp"
#include "OpenGL.hpp"
#include "log.hpp"

FrameBuffer::FrameBuffer(glm::vec2 size) :
    RenderTarget(RenderTarget::Type::FrameBuffer) {
    // Create the framebuffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create the texture we are rendering to
    glGenTextures(1, &texture);
    // Bind it
    glBindTexture(GL_TEXTURE_2D, texture);
    // Make it empty
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, nullptr);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Unbind it again
    glBindTexture(GL_TEXTURE_2D, 0);

    // Attach it to the frame buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           texture, 0);

    // Create the renderbuffer object for depth and stencil testing
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // Attach the render buffer to the framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, rbo);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    check_complete();
}

void FrameBuffer::set_as_active() { glBindFramebuffer(GL_FRAMEBUFFER, fbo); }

void FrameBuffer::check_complete() {
    auto framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebuffer_status == GL_FRAMEBUFFER_COMPLETE) {
        Saturn::log("Framebuffer initialized");
    } else {
        // Yikes
        Saturn::error("Failed to create framebuffer");
        if (framebuffer_status == GL_FRAMEBUFFER_UNDEFINED) {
            Saturn::error("GL_FRAMEBUFFER_UNDEFINED");
        } else if (framebuffer_status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
            Saturn::error("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
        } else if (framebuffer_status ==
                   GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) {
            Saturn::error("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
        } else if (framebuffer_status ==
                   GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER) {
            Saturn::error("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
        } else if (framebuffer_status ==
                   GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER) {
            Saturn::error("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
        } else if (framebuffer_status == GL_FRAMEBUFFER_UNSUPPORTED) {
            Saturn::error("GL_FRAMEBUFFER_UNSUPPORTED");
        } else if (framebuffer_status ==
                   GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) {
            Saturn::error("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
        } else if (framebuffer_status ==
                   GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS) {
            Saturn::error("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
        }
    }
}
