#ifndef SATURN_RESOURCE_TYPES_HPP_
#define SATURN_RESOURCE_TYPES_HPP_

#include "OpenGL.hpp"

#include <cstddef>

namespace Saturn {

namespace Resources {

struct Model {
    GLuint vbo;
    GLuint vao;
    std::size_t vertex_count;
};

struct Texture {
    GLuint handle;
    int unit;
    int width, height;
};

struct Shader
{
	GLuint handle;
};

} // namespace Resources

} // namespace Saturn

#endif
