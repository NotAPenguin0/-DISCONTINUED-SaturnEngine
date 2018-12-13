#ifndef SATURN_RENDER_SYSTEM_HPP_
#define SATURN_RENDER_SYSTEM_HPP_

#include "ComponentSystem.hpp"
#include "Components.hpp"
#include "Shader.hpp"

namespace Saturn {

namespace Systems {

using namespace Components;

class RenderSystem : public ComponentSystem {
public:
    static constexpr const char* defaultVertexPath =
        "resources/shaders/default_v.glsl";
    static constexpr const char* defaultFragmentPath =
        "resources/shaders/default_f.glsl";

    static inline Shader defaultShader;

    SYSTEM_CONSTRUCTOR(RenderSystem)

    using Components = ListComponents<Transform, Mesh>;

    void onStart() override;
    void onUpdate() override;
};

} // namespace Systems

} // namespace Saturn

#endif
