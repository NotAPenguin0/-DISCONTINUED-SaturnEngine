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
    static constexpr const char* defaultVertexPath =
        "resources/shaders/default_v.glsl";
    static constexpr const char* defaultFragmentPath =
        "resources/shaders/default_f.glsl";

    static constexpr const char* texturedVertexPath =
        "resources/shaders/textured_v.glsl";
    static constexpr const char* texturedFragmentPath =
        "resources/shaders/textured_f.glsl";

    SYSTEM_CONSTRUCTOR(RenderSystem)

    using RenderComponents = ListComponents<Transform, Mesh, Shader>;

    void onStart() override;
    void onUpdate() override;

    void setShaderUniforms(Components::Shader& shader,
                           glm::mat4 const& model,
                           glm::mat4 const& view,
                           glm::mat4 const& projection);
};

} // namespace Systems

} // namespace Saturn

#endif
