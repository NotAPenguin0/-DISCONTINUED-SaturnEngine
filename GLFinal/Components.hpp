#ifndef MVG_COMPONENTS_HPP_
#define MVG_COMPONENTS_HPP_

#include <GLM/glm.hpp>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "IDGenerator.hpp"

namespace mvg {

namespace Components {

struct ComponentBase {
    ObjectIDGenerator::IDType object;
};

// Todo: make this an actual transform component (Position, Rotation, Scale) -->
// These apply to the model matrix only
struct Transform : ComponentBase {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

//Temporary component, until there are better alternatives
struct Model : ComponentBase {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> colors;
};

} // namespace Components

namespace detail {

template<class... Cs>
struct AllComponents {};

#define COMPONENT ::mvg::Components::

using MainComponentList =
    AllComponents<COMPONENT Transform, COMPONENT Model>;

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
}; // namespace mvg

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

} // namespace mvg

#endif
