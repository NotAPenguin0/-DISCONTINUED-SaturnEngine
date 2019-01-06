#include "OpenGL.hpp"

std::ostream& operator<<(std::ostream& out, glm::vec3 const& vec) {
    out << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return out;
}

void enable_gl_option(GLenum option) { glEnable(option); }

void disable_gl_option(GLenum option) { glDisable(option); }
