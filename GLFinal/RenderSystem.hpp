#ifndef MVG_RENDER_SYSTEM_HPP_
#define MVG_RENDER_SYSTEM_HPP_

#include "ComponentSystem.hpp"
#include "Components.hpp"

namespace mvg {

namespace Systems {

using namespace Components;

class RenderSystem : public ComponentSystem
{
public:
    SYSTEM_CONSTRUCTOR(RenderSystem)

    using Components = ListComponents<Transform, Model>;

	void onStart() override;
	void onUpdate() override;
};

}

} // namespace mvg

#endif