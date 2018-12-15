#include "LightShader.hpp"

#include <GLM\gtc\type_ptr.hpp>
#include <fstream>
#include <string>

#include "log.hpp"

namespace Saturn {

namespace deleted
	{

LightShader::LightShader(
    const char* vertex /* = DefaultVertexShaderPath */,
    const char* fragment /* = DefaultFragmentShaderPath */) :
    Shader(vertex, fragment) {} // Chain constructors to use base constructor

LightShader::~LightShader() {}

void LightShader::use() const {
    Shader::use(); // Call base version first
    material.diffuseMap->bind();
    material.specularMap->bind();
}

void LightShader::update_uniforms() const {

	Shader::update_uniforms(); //Update MVP matrices

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
    if (loc == -1) Saturn::warning("Uniform"s + s.data() + " not found");
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

}

} // namespace Saturn
