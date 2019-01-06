#ifndef ENGINE_MODEL_HPP_INCLUDE
#define ENGINE_MODEL_HPP_INCLUDE

#include "Shader.hpp"
#include "Mesh.hpp"
#include <filesystem>
#include <vector>

class Model {
public:
    static Model load_from_file(std::filesystem::path const& path);

    void render(Shader& shader);

private:
    std::vector<Mesh> meshes;
};

#endif // !ENGINE_MODEL_HPP_INCLUDE