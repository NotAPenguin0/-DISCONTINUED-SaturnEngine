#include "LightShader.hpp"

#include <GLM\gtc\type_ptr.hpp>
#include <fstream>
#include <string>

#include "log.hpp"

namespace mvg {

namespace detail {
unsigned int create_shader(const char* vtx_path, const char* frag_path) {
    using namespace std::literals::string_literals;

    std::fstream file(vtx_path);

    if (!file.good()) {
        mvg::error(
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
        mvg::error(
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
        mvg::error("[SHADER::VERTEX::COMPILATION_FAILED]: "s + infolog);
        return -1;
    }

    // And again for the fragment shader
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(frag_shader, 512, nullptr, infolog);
        mvg::error("[SHADER::FRAGMENT::COMPILATION_FAILED]: "s + infolog);
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
        mvg::error("[SHADER::LINK_FAILED]: "s + infolog);
        return -1;
    }

    // These are linked now and can safely be deleted
    glDeleteShader(vtx_shader);
    glDeleteShader(frag_shader);

    return shaderProgram;
}
} // namespace detail

LightShader::LightShader(
    const char* vertex /* = DefaultVertexShaderPath */,
    const char* fragment /* = DefaultFragmentShaderPath */) {

    m_handle = detail::create_shader(vertex, fragment);
}

LightShader::~LightShader() { glDeleteProgram(handle()); }

LightShader::LightShader(LightShader&& rhs) :
    m_handle(rhs.m_handle), m_directional(std::move(rhs.m_directional)),
    m_point(std::move(rhs.m_point)), m_spot(std::move(rhs.m_spot)) {
    rhs.m_handle = 0;
}

LightShader& LightShader::operator=(LightShader&& rhs) {

    if (m_handle != rhs.m_handle) {
        m_directional = std::move(rhs.m_directional);
        m_point = std::move(rhs.m_point);
        m_spot = std::move(rhs.m_spot);

        m_handle = rhs.m_handle;
        rhs.m_handle = 0;
    }

    return *this;
}

unsigned int LightShader::handle() const { return m_handle; }

void LightShader::use() const {
    glUseProgram(handle());
    material.diffuseMap->bind();
    material.specularMap->bind();
}

void LightShader::update_uniforms() const {

    // Set view position
    glUniform3fv(location("viewPos"), 1, glm::value_ptr(camera->Position));

    // Set material data
    glUniform3fv(location("material.ambient"), 1,
                 glm::value_ptr(material.ambient));
    glUniform1i(location("material.diffuse"),
                material.diffuseMap->unit() - GL_TEXTURE0);
    glUniform1i(location("material.specular"),
                material.specularMap->unit() - GL_TEXTURE0);
    glUniform1f(location("material.shininess"), material.shininess);

    update_directional();
    update_point();
    update_spot();

    // Update MVP matrices
    glUniformMatrix4fv(location("model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(location("view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(location("projection"), 1, GL_FALSE,
                       glm::value_ptr(projection));
}

void LightShader::update_directional() const {
    glUniform1i(location("directional_size"), m_directional.size());
    // Loop over directional lights, set their uniforms
    for (std::size_t i = 0; i < m_directional.size(); ++i) {
        auto str = std::to_string(i);
        static std::string arr_name = "directional"
                                      "[";
        auto full_name = arr_name + str + "].";

        auto const& light = m_directional[i];

        auto dir = location(full_name + "direction");
        auto amb = location(full_name + "ambient");
        auto diff = location(full_name + "diffuse");
        auto spec = location(full_name + "specular");

        // Set direction, diffuse, ambient and specular values
        glUniform3fv(dir, 1, glm::value_ptr(light->direction));
        glUniform3fv(amb, 1, glm::value_ptr(light->ambient));
        glUniform3fv(diff, 1, glm::value_ptr(light->diffuse));
        glUniform3fv(spec, 1, glm::value_ptr(light->specular));
        glUniform3fv(glGetUniformLocation(m_handle, "debug"), 1,
                     glm::value_ptr(glm::vec3(1.0, 0.0, 0.0)));
    }
}

void LightShader::update_point() const {
    glUniform1i(location("point_size"), m_point.size());

    for (std::size_t i = 0; i < m_point.size(); ++i) {
        auto str = std::to_string(i);
        static std::string arr_name = "point[";
        auto full_name = arr_name + str + "].";

        auto const& light = m_point[i];

        // Set direction, ambient, diffuse and specular values
        glUniform3fv(location(full_name + "position"), 1,
                     glm::value_ptr(light->position));
        glUniform3fv(location(full_name + "ambient"), 1,
                     glm::value_ptr(light->ambient));
        glUniform3fv(location(full_name + "diffuse"), 1,
                     glm::value_ptr(light->diffuse));
        glUniform3fv(location(full_name + "specular"), 1,
                     glm::value_ptr(light->specular));
        // Set attenuation factors
        glUniform1f(location(full_name + "constant"), light->constant);
        glUniform1f(location(full_name + "linear"), light->linear);
        glUniform1f(location(full_name + "quadratic"), light->quadratic);
    }
}

#define xlocation(x) CHECKED_LOCATION(this, x)

GLint CHECKED_LOCATION(LightShader const* instance, std::string_view s) {
    using namespace std::literals::string_literals;

    auto loc = instance->location(s);
    if (loc == -1) mvg::warning("Uniform"s + s.data() + " not found");
    return loc;
}

void LightShader::update_spot() const {
    glUniform1i(location("spot_size"), m_spot.size());
    for (std::size_t i = 0; i < m_spot.size(); ++i) {
        auto str = std::to_string(i);
        static std::string arr_name = "spot[";
        auto full_name = arr_name + str + "].";

        auto const& light = m_spot[i];

        // Set direction, ambient, diffuse and specular values
        glUniform3fv(location(full_name + "position"), 1,
                     glm::value_ptr(light->position));
        glUniform3fv(location(full_name + "direction"), 1,
                     glm::value_ptr(light->direction));
        glUniform3fv(location(full_name + "ambient"), 1,
                     glm::value_ptr(light->ambient));
        glUniform3fv(location(full_name + "diffuse"), 1,
                     glm::value_ptr(light->diffuse));
        glUniform3fv(location(full_name + "specular"), 1,
                     glm::value_ptr(light->specular));
        // Set attenuation factors
        glUniform1f(location(full_name + "constant"), light->constant);
        glUniform1f(location(full_name + "linear"), light->linear);
        glUniform1f(location(full_name + "quadratic"), light->quadratic);
        glUniform1f(location(full_name + "cutOff"), glm::cos(light->radius));
        glUniform1f(location(full_name + "outerCutOff"),
                    glm::cos(light->soft_radius));
    }
}

GLint LightShader::location(std::string_view name) const {
    return glGetUniformLocation(handle(), name.data());
}

} // namespace mvg
