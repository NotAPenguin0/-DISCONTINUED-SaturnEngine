#ifndef MVG_GL_ERRORS_HPP_
#define MVG_GL_ERRORS_HPP_

#include "glad/glad.h"
#include <stdexcept>

#ifdef _DEBUG
inline void check_gl_errors() {
    GLenum error = glGetError();
    if (error == GL_INVALID_ENUM) {
        throw std::runtime_error("GL_INVALID_ENUM");
    } else if (error == GL_INVALID_VALUE) {
        throw std::runtime_error("GL_INVALID_VALUE");
    } else if (error == GL_INVALID_OPERATION) {
        throw std::runtime_error("GL_INVALID_OPERATION");
    } else if (error == GL_INVALID_FRAMEBUFFER_OPERATION) {
        throw std::runtime_error("GL_INVALID_FRAMEBUFFER_OPERATION");
    } else if (error == GL_OUT_OF_MEMORY) {
        throw std::runtime_error("GL_OUT_OF_MEMORY");
    }
}
#    define CHECK_GL_ERRORS() check_gl_errors()
#else
#    define CHECK_GL_ERRORS()
#endif // _DEBUG

#endif