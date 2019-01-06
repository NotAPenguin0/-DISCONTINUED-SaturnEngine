#ifndef MVG_TEXTURED_RENDERER_HPP_
#define MVG_TEXTURED_RENDERER_HPP_

#include "AbstractRenderer.hpp"
#include "TexturedObject.hpp"

class TexturedRenderer : public AbstractRenderer {
public:
	void render(TexturedObject& obj, RenderTarget& render_target);
};

#endif