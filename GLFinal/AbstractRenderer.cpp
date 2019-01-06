#include "AbstractRenderer.hpp"

#include <stdexcept>

void AbstractRenderer::set_shader(Shader& shader) { active_shader = &shader; }


void AbstractRenderer::render(Mesh& mesh, RenderTarget& render_target) {
    if (active_shader == nullptr) { throw std::runtime_error("No shader set"); }
    active_shader->use();
    render_target.set_as_active();
    mesh.render(*active_shader);
}

void AbstractRenderer::update_matrix_uniforms(int matrices_to_update) {
    if (active_shader == nullptr) throw std::runtime_error("No shader set");
    if (matrices_to_update & MatrixNames::Model) {
        active_shader->set_mat4("model", render_matrices.model);
    }
    if (matrices_to_update & MatrixNames::View) {
        active_shader->set_mat4("view", render_matrices.view);
    }
    if (matrices_to_update & MatrixNames::Projection) {
        active_shader->set_mat4("projection", render_matrices.projection);
    }
}
