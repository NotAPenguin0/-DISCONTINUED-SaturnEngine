#include "RenderSystem.hpp"

namespace mvg::Systems {

void RenderSystem::onStart()
{
	//This only needs to happen to objects that have a model component
	for (auto ptr : getObjects<ListComponents<Model>>())
	{ 
		//get a reference for simplicity
		auto& obj = *ptr;
        auto& model = obj.getComponent<Model>();
		
	}
}

void RenderSystem::onUpdate() {
    for (auto ptr : getObjects<Components>()) { 
		//Get a reference for simplicity
		auto& obj = *ptr; 
		auto& transform = obj.getComponent<Transform>();
        auto& model = obj.getComponent<Model>();
	}
}

} // namespace mvg::Systems
