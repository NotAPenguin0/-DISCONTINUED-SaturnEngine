#include "RenderTarget.hpp"

#include "OpenGL.hpp"

RenderTarget::RenderTarget(Type t) : type(t) {}

RenderTarget::Type RenderTarget::get_type() const { return type; }

void RenderTarget::clear(glm::vec3 color, GLenum buffers)
{
    set_as_active();
    glClearColor(color.x, color.y, color.z, 1.0f);
	glClear(buffers);
}