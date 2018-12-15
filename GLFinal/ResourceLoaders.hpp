#ifndef SATURN_RESOURCE_LOADERS_HPP_
#define SATURN_RESOURCE_LOADERS_HPP_

#include "Components.hpp"
#include "OpenGL.hpp"
#include "Resource.hpp"
#include "ResourceTypes.hpp"

#include <fstream>

namespace Saturn {

class ResourceLoaders {
public:
    static bool loadModel(std::string path, Resources::Model& model);
    static void unloadModel(Resources::Model& model);

    static bool loadTexture(std::string path, Resources::Texture& texture);
    static void unloadTexture(Resources::Texture& texture);

    static bool loadShader(std::string path, Resources::Shader& shader);
    static void unloadShader(Resources::Shader& shader);
};

} // namespace Saturn

#endif
