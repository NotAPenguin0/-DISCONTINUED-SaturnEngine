#ifndef SATURN_COMPONENTS_HPP_
#define SATURN_COMPONENTS_HPP_

#include <GLM/glm.hpp>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "IDGenerator.hpp"
#include "Light.hpp"
#include "OpenGL.hpp"
#include "Resource.hpp"
#include "ResourceTypes.hpp"


namespace Saturn {

class FPSCamera;

// Until we start using Quaternions
struct RotationData {
    glm::vec3 axis = glm::vec3(1.0f, 1.0f, 1.0f);
    float angle = 0.0f;
};

namespace Components {

struct ComponentBase {
    ObjectIDGenerator::IDType object;
};

struct Transform : ComponentBase {
    glm::vec3 position;
    RotationData rotation;
    glm::vec3 scale;
};

// Temporary component, until there are better alternatives
struct Mesh : ComponentBase {
    ResourceRef<Resources::Model> model;
};

struct Material : ComponentBase {
    ResourceRef<Resources::Texture> texture = null_resource;

    struct Lighting {
        ResourceRef<Resources::Texture> diffuseMap = null_resource;
        ResourceRef<Resources::Texture> specularMap = null_resource;
        glm::vec3 ambient;
        float shininess;
    } lighting;
};

struct Shader : ComponentBase {
    ResourceRef<Resources::Shader> shader;
    // Specifies whether lighting should affect the object this component is
    // attached to
    bool lit = true;
};

struct Lights : ComponentBase {
    std::vector<DirectionalLight> directionalLights;
    std::vector<PointLight> pointLights;
    std::vector<SpotLight> spotLights;
};

// If an object has a CamLight, we assume it has a Lights component, with the
// camera light at spotLights[0]
struct CamLight : ComponentBase {
    FPSCamera* camera;
};

} // namespace Components

namespace detail {

template<class... Cs>
struct AllComponents {};

#define COMPONENT ::Saturn::Components::

using MainComponentList = AllComponents<COMPONENT Transform,
                                        COMPONENT Mesh,
                                        COMPONENT Lights,
                                        COMPONENT Shader,
                                        COMPONENT Material,
                                        COMPONENT CamLight>;

#undef COMPONENT

} // namespace detail

// Requires that the component is default-constructible
template<class C>
struct ComponentList {
    using IdT = ObjectIDGenerator::IDType;

    void add(IdT objId, C c = C{}) {
        c.object = objId;
        components[objId] = std::move(c);
    }

    void remove(IdT objId) {
        if (components.find(objId) != components.end()) {
            components.erase(objId);
        }
    }

    C& get(IdT objId) { return components[objId]; }

    C const& get(IdT objId) const { return components.at(objId); }

    std::unordered_map<IdT, C> components;
}; // namespace Saturn

template<typename C>
ComponentList<C> components;

namespace detail {

template<class CList>
struct cleanup;

template<class... Cs>
struct cleanup<AllComponents<Cs...>> {
private:
    template<ObjectIDGenerator::IDType I>
    void do_cleanup(ObjectIDGenerator::IDType id) {
        components<std::tuple_element_t<I, std::tuple<Cs...>>>.remove(id);
        do_cleanup<I - 1>(id);
    }

    template<>
    void do_cleanup<0>(ObjectIDGenerator::IDType id) {
        components<std::tuple_element_t<0, std::tuple<Cs...>>>.remove(id);
    }

public:
    void operator()(ObjectIDGenerator::IDType id) {
        do_cleanup<sizeof...(Cs) - 1>(id);
    }
};

} // namespace detail

inline void delete_components_for_index(ObjectIDGenerator::IDType id) {
    detail::cleanup<detail::MainComponentList> c;
    c(id);
}

} // namespace Saturn

#endif
