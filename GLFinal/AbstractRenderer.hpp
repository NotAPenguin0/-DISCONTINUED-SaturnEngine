#ifndef MVG_ABSTRACT_RENDERER_HPP_
#define MVG_ABSTRACT_RENDERER_HPP_

#include "Mesh.hpp"
#include "RenderTarget.hpp"
#include "Shader.hpp"

#include <GLM/glm.hpp>

class AbstractRenderer {
public:
    // Todo: Use a helper class for flags
    enum MatrixNames {
        Model = 0x01,
        View = 0x02,
        Projection = 0x04,
        All = Model | View | Projection,
        None = 0x00
    };

	virtual ~AbstractRenderer() = default;

    void set_shader(Shader& shader);
    void render(Mesh& mesh, RenderTarget& render_target);

    struct RenderMatrices {
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
    } render_matrices;

    void update_matrix_uniforms(int matrices_to_update);

protected:
    Shader* active_shader = nullptr;
};

#endif
