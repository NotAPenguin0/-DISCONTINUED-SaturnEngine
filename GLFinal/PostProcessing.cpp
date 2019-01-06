#include "PostProcessing.hpp"

#include <fstream>

PostProcessingShaders::PostProcessingShaders(const char* fname) {
    std::ifstream file(fname);
    if (!file.good())
        throw std::runtime_error(std::string("Failed to open file at path ") + fname);

	/*A PostProcessingShader file contains of entries like:
	  [effect_name] path_to_fragment_shader
	  [effect_name2] paht_to_other_fragment_shader
	  ...etc
	*/

    std::string name;
    while (file >> name) {
        std::string frag_path;
        file >> frag_path;
        shaders[name] = Shader(vertex_path, frag_path.c_str());
    }
}

void PostProcessingShaders::add_shader(std::string const& name, Shader&& shader)
{
    shaders[std::string(name)] = std::move(shader);
}

Shader& PostProcessingShaders::get(std::string const& name)
{
	return shaders.at(name);
}