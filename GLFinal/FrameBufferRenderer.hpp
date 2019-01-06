#ifndef MVG_FRAMEBUFFER_RENDERER_HPP_
#define MVG_FRAMEBUFFER_RENDERER_HPP_

#include "AbstractRenderer.hpp"
#include "FrameBuffer.hpp"
#include "RenderTarget.hpp"

class FrameBufferRenderer : public AbstractRenderer {
public:
    FrameBufferRenderer();

    virtual ~FrameBufferRenderer();

    void set_postprocessing_shader(Shader& shader);

    void render(FrameBuffer& buffer, RenderTarget& target);

private:
    unsigned int screen_vao;
    unsigned int screen_vbo;

    Shader* postprocessing_shader = nullptr;
};

#endif
