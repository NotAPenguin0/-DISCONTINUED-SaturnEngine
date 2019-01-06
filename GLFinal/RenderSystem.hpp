#ifndef SATURN_RENDER_SYSTEM_HPP_
#define SATURN_RENDER_SYSTEM_HPP_

#include "ComponentSystem.hpp"
#include "Components.hpp"
#include "Resource.hpp"
#include "ResourceTypes.hpp"

namespace Saturn {

namespace Systems {

using namespace Components;

class RenderSystem : public ComponentSystem {
public:
    static constexpr unsigned int MainTextureUnit = 0;
    static constexpr unsigned int DiffuseMapTextureUnit = 0;
    static constexpr unsigned int SpecularMapTextureUnit = 1;

    ResourceRef<Resources::Shader> lightShader;

    SYSTEM_CONSTRUCTOR(RenderSystem)

    using RenderComponents = ListComponents<Transform, Mesh, Shader, Material>;

    void onStart() override;
    void onUpdate() override;

    void setShaderUniforms(Components::Shader& shader,
                           glm::mat4 const& model,
                           glm::mat4 const& view,
                           glm::mat4 const& projection);
    void setupLightingUniforms(Components::Shader& shader);
    void directionalUniforms(Components::Shader& shader,
                             std::vector<DirectionalLight*> const& lights);
    void pointUniforms(Components::Shader& shader,
                       std::vector<PointLight*> const& lights);
    void spotUniforms(Components::Shader& shader,
                      std::vector<SpotLight*> const& lights);

    int location(Components::Shader& shader, std::string name);
};

} // namespace Systems

} // namespace Saturn

#endif
