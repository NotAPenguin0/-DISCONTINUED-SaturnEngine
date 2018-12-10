#ifndef MVG_LIGHT_SHADER_HPP_
#define MVG_LIGHT_SHADER_HPP_

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <type_traits>
#include <vector>

#include "FPSCamera.hpp"
#include "Light.hpp"
#include "Material.hpp"

namespace mvg {

namespace detail {

unsigned int create_shader(const char* vtx_path, const char* frag_path);

} // namespace detail

class LightShader {
public:
    LightShader(const char* vertex = DefaultVertexShaderPath,
                const char* fragment = DefaultFragmentShaderPath);
    LightShader(LightShader const&) = delete;
    LightShader& operator=(LightShader const&) = delete;

    LightShader(LightShader&&);
    LightShader& operator=(LightShader&&);

    ~LightShader();

    unsigned int handle() const;

    void use() const;
    void update_uniforms() const;

    template<class LightType>
    void add(LightType* l) {
        if constexpr (std::is_same_v<LightType, DirectionalLight>) {
            m_directional.push_back(l);
        }
        if constexpr (std::is_same_v<LightType, PointLight>) {
            m_point.push_back(l);
        }
        if constexpr (std::is_same_v<LightType, SpotLight>) {
            m_spot.push_back(l);
        }
    }

    GLint location(std::string_view name) const;

    // MVP matrices. Assign these first, and call update_uniforms() afterwards
    // to apply the changes in the shader.
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    // update_uniforms() has to be called to apply changes
    Material material;

    // Pointer to the camera so you don't have to manually update the view
    // position every frame
    FPSCamera* camera;

private:
    static constexpr const char* DefaultFragmentShaderPath =
        "shaders/light_default_fragment.glsl";
    static constexpr const char* DefaultVertexShaderPath =
        "shaders/light_default_vertex.glsl";

    unsigned int m_handle;

	void update_directional() const;
    void update_point() const;
	void update_spot() const;

    std::vector<DirectionalLight*> m_directional;
    std::vector<PointLight*> m_point;
    std::vector<SpotLight*> m_spot;
};

} // namespace mvg

#endif
