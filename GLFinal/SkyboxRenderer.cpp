#include "SkyboxRenderer.hpp"

void SkyboxRenderer::render(Skybox& skybox, RenderTarget& target)
{
    target.set_as_active();
	disable_gl_option(GL_CULL_FACE); //No face culling when rendering the skybox
    glDepthMask(0x00); // Disable writing to the depth buffer
    glUseProgram(active_shader->handle());
    glBindVertexArray(skybox.vao);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.cubemap);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    enable_gl_option(GL_CULL_FACE);
    glDepthMask(0xff); // re enable writing to the depth buffer
}