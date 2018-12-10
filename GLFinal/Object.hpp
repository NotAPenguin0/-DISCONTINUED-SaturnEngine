#ifndef MVG_OBJECT_HPP_
#define MVG_OBJECT_HPP_

#include "Components.hpp"
#include "IDGenerator.hpp"
#include <cstddef>

namespace mvg {

class Object {
public:
    Object() = default;
    inline Object(ObjectIDGenerator::IDType id) : ID(id) {}
    ~Object();
    inline ObjectIDGenerator::IDType id() const { return ID; }

    template<typename C>
    void addComponent() {
        components<C>.add(ID);
	}

    template<typename C>
	void addComponent(C const& c)
	{
        components<C>.add(ID, c);
	}

	template<typename C>
	C& getComponent()
	{
        return components<C>.get(ID);
	}

	template<typename C>
	C const& getComponent() const
	{
        return components<C>.get(ID);
	}

	template<typename C>
	bool hasComponent() const {
		//Try to find this ID in the component list associated with the component to be found
        return (components<C>.components.find(ID) !=
                components<C>.components.end());
	}

private:
    static inline ObjectIDGenerator idgen;
    ObjectIDGenerator::IDType ID;
};

} // namespace mvg

#endif
