#include "Plane.hpp"

#include "OpenGL.hpp"

Plane::Plane(float scale /*= 1.0f*/) :
    Mesh({Vertex(glm::vec3(-1.0f * scale, 1.0f * scale, 0.0f),
                 glm::vec3(0.0f, 0.0f, -1.0f),
                 glm::vec2(0.0f, 1.0f * scale)),
          Vertex(glm::vec3(-1.0f * scale, -1.0f * scale, 0.0f),
                 glm::vec3(0.0f, 0.0f, -1.0f),
                 glm::vec2(0.0f, 0.0f)),
          Vertex(glm::vec3(1.0f * scale, -1.0f * scale, 0.0f),
                 glm::vec3(0.0f, 0.0f, -1.0f),
                 glm::vec2(1.0f * scale, 0.0f)),
          Vertex(glm::vec3(-1.0f * scale, 1.0f * scale, 0.0f),
                 glm::vec3(0.0f, 0.0f, -1.0f),
                 glm::vec2(0.0f, 1.0f * scale)),
          Vertex(glm::vec3(1.0f * scale, -1.0f * scale, 0.0f),
                 glm::vec3(0.0f, 0.0f, -1.0f),
                 glm::vec2(1.0f * scale, 0.0f)),
          Vertex(glm::vec3(1.0f * scale, 1.0f * scale, 0.0f),
                 glm::vec3(0.0f, 0.0f, -1.0f),
                 glm::vec2(1.0f * scale, 1.0f * scale))},
         {0, 1, 2, 3, 4, 5},
         {}) {}

void Plane::render(Shader& shader, bool instanced, std::size_t count) {
    // Disable face culling for rendering planes, because otherwise the back of
    // the plane will not be rendered
    disable_gl_option(GL_CULL_FACE);
    Mesh::render(shader, instanced, count);
    enable_gl_option(GL_CULL_FACE);
}
