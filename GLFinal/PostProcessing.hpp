#ifndef MVG_POST_PROCESSING_HPP_
#define MVG_POST_PROCESSING_HPP_

#include "Shader.hpp"

#include <map>
#include <string>

class PostProcessingShaders {
public:
    static constexpr const char* vertex_path =
        "resources/shaders/postprocessing/postprocess_v.glsl";

    PostProcessingShaders(const char* fname);

    void add_shader(std::string const& name, Shader&& shader);

    Shader& get(std::string const& name);

private:
    std::map<std::string, Shader> shaders;
};

#endif
