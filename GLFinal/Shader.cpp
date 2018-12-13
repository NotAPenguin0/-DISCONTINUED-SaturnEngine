#include "Shader.hpp"

#include <fstream>
#include <string>

#include <GLM/gtc/type_ptr.hpp>

#include "log.hpp"

namespace Saturn {

namespace detail {

unsigned int create_shader(const char* vtx_path, const char* frag_path) {
    using namespace std::literals::string_literals;

    std::fstream file(vtx_path);

    if (!file.good()) {
        Saturn::error(
            "[SHADER::VERTEX]: failed to open vertex shader source file at path"s +
            vtx_path);
        return -1;
    }

    std::stringstream buf;
    buf << file.rdbuf();

    std::string vtx_source(buf.str());

    file.close();
    buf = std::stringstream{}; // reset buffer
    file.open(frag_path);

    if (!file.good()) {
        Saturn::error(
            "[SHADER::FRAGMENT]: failed to open fragment shader source file at path"s +
            frag_path);
        return -1;
    }

    buf << file.rdbuf();

    std::string frag_source(buf.str());

    unsigned int vtx_shader, frag_shader;
    vtx_shader = glCreateShader(GL_VERTEX_SHADER);
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    // This is wrapped inside a lambda to limit the scope of vtx_carr and
    // frag_carr
    [&vtx_source, &frag_source, &vtx_shader, &frag_shader]() {
        const char* vtx_carr = vtx_source.c_str();
        const char* frag_carr = frag_source.c_str();
        glShaderSource(vtx_shader, 1, &vtx_carr, nullptr);
        glShaderSource(frag_shader, 1, &frag_carr, nullptr);
    }();

    glCompileShader(vtx_shader);
    glCompileShader(frag_shader);

    // Now, check for compilation errors
    int success;
    char infolog[512];
    glGetShaderiv(vtx_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vtx_shader, 512, nullptr, infolog);
        Saturn::error("[SHADER::VERTEX::COMPILATION_FAILED]: "s + infolog);
        return -1;
    }

    // And again for the fragment shader
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(frag_shader, 512, nullptr, infolog);
        Saturn::error("[SHADER::FRAGMENT::COMPILATION_FAILED]: "s + infolog);
        return -1;
    }

    // Finally, link the vertex and fragment shader together
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vtx_shader);
    glAttachShader(shaderProgram, frag_shader);
    glLinkProgram(shaderProgram);

    // Check for errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infolog);
        Saturn::error("[SHADER::LINK_FAILED]: "s + infolog);
        return -1;
    }

    // These are linked now and can safely be deleted
    glDeleteShader(vtx_shader);
    glDeleteShader(frag_shader);

    return shaderProgram;
}

} // namespace detail

Shader::Shader(const char* vtx, const char* frag) {
    m_handle = detail::create_shader(vtx, frag);
}

Shader::Shader(Shader&& rhs) : m_handle(rhs.m_handle) { rhs.m_handle = 0; }

Shader& Shader::operator=(Shader&& rhs) {
    m_handle = rhs.m_handle;
    rhs.m_handle = 0;
    return *this;
}

Shader::~Shader() { glDeleteProgram(m_handle); }

unsigned int Shader::handle() const { return m_handle; }

void Shader::use() const { glUseProgram(m_handle); }

GLint checked_loc(Shader const* s, std::string_view n)
{
    auto l = glGetUniformLocation(s->handle(), n.data());
    if (l == -1) throw std::runtime_error("uniform not found");
    return l;
}

#define xlocation(n) checked_loc(this, n)

void Shader::update_uniforms() const {

    // Update MVP matrices
    glUniformMatrix4fv(location("model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(location("view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(location("projection"), 1, GL_FALSE,
                       glm::value_ptr(projection));
}

GLint Shader::location(std::string_view name) const {
    return glGetUniformLocation(handle(), name.data());
}

} // namespace Saturn
