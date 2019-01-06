#ifndef MVG_FRAME_BUFFER_HPP_
#define MVG_FRAME_BUFFER_HPP_

#include "RenderTarget.hpp"

#include <GLM/glm.hpp>

class FrameBuffer : public RenderTarget {
public:
    FrameBuffer(glm::vec2 size);

    void set_as_active() override;

	friend class FrameBufferRenderer;

private:
    void check_complete();
	
    unsigned int fbo;
    unsigned int rbo;
    unsigned int texture;
};

#endif
