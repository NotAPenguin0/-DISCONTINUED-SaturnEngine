#include "World.hpp"

namespace mvg {

World::World() { objects.set_world(this); }

void World::onUpdate() {
    for (auto& s : systems) {
        // Update all systems
        s->onUpdate();
    }
}

void World::onStart() {
    for (auto& s : systems) {
        // Make sure all systems are initialized
        s->onStart();
    }
}

} // namespace mvg
