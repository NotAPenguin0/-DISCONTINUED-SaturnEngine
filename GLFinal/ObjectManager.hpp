#ifndef SATURN_OBJECT_MANAGER_HPP_
#define SATURN_OBJECT_MANAGER_HPP_

#include <unordered_map>

#include "IDGenerator.hpp"
#include "Object.hpp"

namespace Saturn {

class World;


class ObjectManager {
public:
    friend class ComponentSystem;

    using IDType = ObjectIDGenerator::IDType;

	using storage_type = std::unordered_map<IDType, Object>;

    Object& get(IDType id);
    Object const& get(IDType id) const;

    void add(Object const& o);
    void remove(IDType id);

    template<typename... Args>
    Object& emplace(Args&&... args) {
        auto id = idgen.next();
        objects[id] = Object(id, std::forward<Args>(args)...);
        return objects[id];
    }

	World& world();
    World const& world() const;

private:
    friend class World;

	void set_world(World* wptr);

    std::unordered_map<IDType, Object> objects;
    ObjectIDGenerator idgen;
    World* w;

};

} // namespace Saturn

#endif
