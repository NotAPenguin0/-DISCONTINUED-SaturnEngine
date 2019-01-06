#include "ScreenRenderTarget.hpp"
#include "OpenGL.hpp"

ScreenRenderTarget::ScreenRenderTarget() : RenderTarget(RenderTarget::Type::Screen)
{

}

void ScreenRenderTarget::set_as_active()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0); //0 means 'render to the screen'
}