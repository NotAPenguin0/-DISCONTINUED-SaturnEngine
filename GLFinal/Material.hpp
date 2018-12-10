#ifndef MVG_MATERIAL_HPP_
#define MVG_MATERIAL_HPP_

#include "Texture.hpp"
#include <GLM/glm.hpp>

namespace mvg {

struct Material {
    glm::vec3 ambient;

    Texture* diffuseMap;
    Texture* specularMap;

    float shininess;
};

} // namespace mvg

#endif
