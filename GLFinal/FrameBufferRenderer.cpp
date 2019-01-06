#include "FrameBufferRenderer.hpp"
#include "OpenGL.hpp"

#include <stdexcept>

static const float screen_vertices[] = {
    -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, // TL
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // BL
    1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, // BR
    -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, // Second triangle TL
    1.0f,  1.0f,  0.0f, 1.0f, 1.0f, // TR
    1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, // Second triangle BR

};

FrameBufferRenderer::FrameBufferRenderer() {
    glGenVertexArrays(1, &screen_vao);
    glGenBuffers(1, &screen_vbo);
    glBindVertexArray(screen_vao);
    glBindBuffer(GL_ARRAY_BUFFER, screen_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertices), screen_vertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
}

FrameBufferRenderer::~FrameBufferRenderer() {
    glDeleteVertexArrays(1, &screen_vao);
    glDeleteBuffers(1, &screen_vbo);
}

void FrameBufferRenderer::set_postprocessing_shader(Shader& shader) {
    postprocessing_shader = &shader;
}

void FrameBufferRenderer::render(FrameBuffer& buffer, RenderTarget& target) {
    if (postprocessing_shader == nullptr)
        throw std::runtime_error("No postprocessing shader set");
    target.set_as_active();
    glBindVertexArray(screen_vao);
    target.clear(glm::vec3(1.0f, 1.0f, 1.0f), GL_COLOR_BUFFER_BIT);
    postprocessing_shader->use();
    disable_gl_option(GL_DEPTH_TEST);
    disable_gl_option(GL_CULL_FACE);
    glBindTexture(GL_TEXTURE_2D, buffer.texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    enable_gl_option(GL_DEPTH_TEST);
    enable_gl_option(GL_CULL_FACE);
}
