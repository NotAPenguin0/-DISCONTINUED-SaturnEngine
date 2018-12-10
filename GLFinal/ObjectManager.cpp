#include "ObjectManager.hpp"
#include "World.hpp"

namespace mvg {

Object& ObjectManager::get(IDType id) { return objects[id]; }

Object const& ObjectManager::get(IDType id) const { return objects.at(id); }

void ObjectManager::add(Object const& o) {
    auto id = idgen.next();
    objects[id] = o;
}

void ObjectManager::remove(IDType id) { objects.erase(id); }

World& ObjectManager::world() { return *w; }

World const& ObjectManager::world() const { return *w; }

void ObjectManager::set_world(World* wptr)
{ w = wptr; }

} // namespace mvg
