#ifndef MVG_SKYBOX_RENDERER_HPP_
#define MVG_SKYBOX_RENDERER_HPP_

#include "Skybox.hpp"
#include "Shader.hpp"
#include "AbstractRenderer.hpp"

class SkyboxRenderer : public AbstractRenderer {
public:
    void render(Skybox& skybox, RenderTarget& target);
};

#endif