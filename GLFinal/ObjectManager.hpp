#ifndef MVG_OBJECT_MANAGER_HPP_
#define MVG_OBJECT_MANAGER_HPP_

#include <unordered_map>

#include "IDGenerator.hpp"
#include "Object.hpp"

namespace mvg {

class World;


class ObjectManager {
public:
    friend class ComponentSystem;

    using IDType = ObjectIDGenerator::IDType;

    Object& get(IDType id);
    Object const& get(IDType id) const;

    void add(Object const& o);
    void remove(IDType id);

    template<typename... Args>
    IDType emplace(Args&&... args) {
        auto id = idgen.next();
        objects[id] = Object(id, std::forward<Args>(args)...);
        return id;
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

} // namespace mvg

#endif
