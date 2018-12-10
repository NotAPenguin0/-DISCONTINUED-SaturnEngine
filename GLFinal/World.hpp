#ifndef MVG_WORLD_HPP_
#define MVG_WORLD_HPP_

#include "ComponentSystem.hpp"
#include "Object.hpp"
#include "ObjectManager.hpp"

#include <memory>

namespace mvg {

class World {
public:
    World();

    template<class S>
    void addSystem() {
        static_assert(
            std::is_base_of_v<ComponentSystem, S>,
            "The system mush be a derived class from ComponentSystem");
        systems.push_back(std::make_unique<S>());
    }

    void onUpdate();
    void onStart();

private:
    ObjectManager objects;
    std::list<std::unique_ptr<ComponentSystem>> systems;
};

} // namespace mvg

#endif