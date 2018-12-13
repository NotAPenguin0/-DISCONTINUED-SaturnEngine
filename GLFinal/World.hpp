#ifndef SATURN_WORLD_HPP_
#define SATURN_WORLD_HPP_

#include "ComponentSystem.hpp"
#include "Object.hpp"
#include "ObjectManager.hpp"

#include <memory>

namespace Saturn {

class World {
public:
    World();

    template<class S>
    void addSystem() {
        static_assert(
            std::is_base_of_v<ComponentSystem, S>,
            "The system mush be a derived class from ComponentSystem");
        systems.push_back(std::make_unique<S>(objects));
    }

    void onUpdate();
    void onStart();

private:
	friend class Application;

    ObjectManager objects;
    std::list<std::unique_ptr<ComponentSystem>> systems;
};

} // namespace Saturn

#endif