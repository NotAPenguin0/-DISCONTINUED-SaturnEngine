#ifndef SATURN_COMPONENT_SYSTEM_HPP_
#define SATURN_COMPONENT_SYSTEM_HPP_

#include "Components.hpp"
#include "ObjectManager.hpp"
#include "type_list.hpp"

#include <list>

namespace Saturn {

class ComponentSystem {
public:
    template<class... Cs>
    using ListComponents = mvg::type_list<Cs...>;

private:
    template<typename CList>
    struct get_objects_impl;

    template<typename... Cs>
    struct get_objects_impl<ListComponents<Cs...>> {
        static void get(std::list<Object*>& result, ObjectManager::storage_type& objects) {
            for (auto& [id, obj] : objects) {
                if ((obj.hasComponent<Cs>() && ...)) {
                    // The object has all components needed for this system to
                    // operate, so add it to the result list
                    result.push_back(&obj);
                }
            }
        }
    };

public:
    inline ComponentSystem(ObjectManager& objects) : objects(objects) {}

    template<class CList>
    std::list<Object*> getObjects() {
        std::list<Object*> result;

        get_objects_impl<CList>::get(result, objects.objects);

        return result;
    }

    // I don't like virtual function calls, but I think I'll need it here.

    inline virtual void onStart() {}
    inline virtual void onUpdate() {}

    virtual ~ComponentSystem() = default;

private:
    ObjectManager& objects;
};

#define SYSTEM_CONSTRUCTOR(name)                                               \
    explicit inline name(ObjectManager& man) : ComponentSystem(man) {}

} // namespace Saturn

#endif
