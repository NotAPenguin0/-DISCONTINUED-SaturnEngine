#include "TexturedRenderer.hpp"

void TexturedRenderer::render(TexturedObject& obj, RenderTarget& render_target)
{
    render_target.set_as_active();
	obj.render(*active_shader);
}