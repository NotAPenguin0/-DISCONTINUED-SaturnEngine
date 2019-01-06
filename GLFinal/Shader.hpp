#ifndef MVG_SHADER_HPP_
#define MVG_SHADER_HPP_

#include <GLM/glm.hpp>

#include <string_view>

class Shader {
public:
    Shader() = default;
    Shader(const char* vtx_source, const char* frag_source);
    Shader(Shader&& rhs);
	Shader& operator=(Shader&& rhs);

    unsigned int handle();
    void use();

    void set_int(std::string_view name, int value);
    void set_float(std::string_view name, float value);
    //#Optimize maybe const ref or ref
    void set_vec3(std::string_view name, glm::vec3 value);
    void set_vec4(std::string_view name, glm::vec4 value);
    void set_mat4(std::string_view name, glm::mat4 value);

    int location(std::string_view name);

private:
    unsigned int program;
};

#endif
