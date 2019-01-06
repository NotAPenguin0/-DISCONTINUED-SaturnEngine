#ifndef SATURN_SHADER_HPP_
#define SATURN_SHADER_HPP_

#include "NonCopyable.hpp"
#include "OpenGL.hpp"

#include <string_view>

#include <GLM/glm.hpp>

namespace Saturn {

namespace deleted {

namespace detail {
unsigned int create_shader(const char* vtx_path, const char* frag_path);
}

class Shader : public NonCopyable {
public:
    Shader() = default;
    Shader(const char* vtx, const char* frag);
    Shader(Shader&& rhs);
    Shader& operator=(Shader&& rhs);
    virtual ~Shader();

    unsigned int handle() const;
    virtual void use() const;
    virtual void update_uniforms() const;

    GLint location(std::string_view name) const;

    glm::mat4 model = glm::mat4(1.0);
    glm::mat4 view = glm::mat4(1.0);
    glm::mat4 projection = glm::mat4(1.0);

private:
    GLuint m_handle;
};

} // namespace deleted

} // namespace Saturn

#endif
