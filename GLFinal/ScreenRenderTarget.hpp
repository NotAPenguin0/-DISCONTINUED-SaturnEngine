#ifndef MVG_SCREEN_RENDER_TARGET_HPP_
#define MVG_SCREEN_RENDER_TARGET_HPP_

#include "RenderTarget.hpp"

class ScreenRenderTarget : public RenderTarget {
public:
    ScreenRenderTarget();

	void set_as_active() override;
};

#endif