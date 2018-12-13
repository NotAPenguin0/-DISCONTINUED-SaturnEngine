#ifndef SATURN_LIGHT_HPP_
#define SATURN_LIGHT_HPP_

#include <GLM/glm.hpp>

namespace Saturn {
struct Light {
    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    // #TODO: This is for later. Multiply all vectors with this value to get
    // final light value
    float intensity = 1.0f;
};

struct DirectionalLight : public Light {
    glm::vec3 direction;
};

struct PointLight : public Light {
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight : public PointLight {
    glm::vec3 direction;

    float radius;
    float soft_radius;
};

} // namespace Saturn

#endif
