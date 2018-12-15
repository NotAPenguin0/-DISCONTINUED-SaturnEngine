#ifndef SATURN_LIGHT_SHADER_HPP_
#define SATURN_LIGHT_SHADER_HPP_

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <type_traits>
#include <vector>

#include "FPSCamera.hpp"
#include "Light.hpp"
#include "Material.hpp"

#include "Shader.hpp"

namespace Saturn {

namespace deleted {

class LightShader : public Shader {
public:
    LightShader(const char* vertex = DefaultVertexShaderPath,
                const char* fragment = DefaultFragmentShaderPath);

    LightShader(LightShader&&) = default;
    LightShader& operator=(LightShader&&) = default;

    virtual ~LightShader();

    void use() const override;
    void update_uniforms() const override;

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

    void update_directional() const;
    void update_point() const;
    void update_spot() const;

    std::vector<DirectionalLight*> m_directional;
    std::vector<PointLight*> m_point;
    std::vector<SpotLight*> m_spot;
};

} // namespace deleted

} // namespace Saturn

#endif
