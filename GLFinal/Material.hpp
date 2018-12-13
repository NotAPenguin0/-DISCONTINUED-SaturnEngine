#ifndef SATURN_MATERIAL_HPP_
#define SATURN_MATERIAL_HPP_

#include "Texture.hpp"
#include <GLM/glm.hpp>

namespace Saturn {

struct Material {
    glm::vec3 ambient;

    Texture* diffuseMap;
    Texture* specularMap;

    float shininess;
};

} // namespace Saturn

#endif
