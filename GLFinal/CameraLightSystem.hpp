#ifndef SATURN_CAMERA_LIGHT_SYSTEM_HPP_
#define SATURN_CAMERA_LIGHT_SYSTEM_HPP_

#include "ComponentSystem.hpp"

namespace Saturn::Systems {

class CameraLightSystem : public ComponentSystem {
public:
    SYSTEM_CONSTRUCTOR(CameraLightSystem)

    using Components = ListComponents<Components::CamLight>;

    void onUpdate() override;
};

} // namespace Saturn::Systems

#endif
