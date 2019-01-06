#include "CameraLightSystem.hpp"

#include "Globals.hpp"
#include "FPSCamera.hpp"

namespace Saturn::Systems {

void CameraLightSystem::onUpdate() {
    using namespace Saturn::Components;

    for (auto& obj : getObjects<Components>()) {
        auto& cam = obj->getComponent<CamLight>();
        auto& light = obj->getComponent<Lights>().spotLights[0];
        light.position = cam.camera->Position;
		light.direction = cam.camera->Front;
    }
}

} // namespace Saturn::Systems
