#ifndef MVG_INSTANCED_MESH_HPP_
#define MVG_INSTANCED_MESH_HPP_

#include "Mesh.hpp"
#include "OpenGL.hpp"
#include "Renderable.hpp"

#include <type_traits>
#include <vector>

template<typename R>
class InstancedMesh {
public:
	static_assert(std::is_base_of_v<Renderable, R>, "Type must be a Renderable");

    // The InstancedMesh class stores a pointer to the mesh. When rendering, the
    // mesh will be rendered using instancing
    InstancedMesh(R& m, std::vector<glm::vec3> const& positions) {
        mesh = &m;
        // Fill position buffer data
        glGenBuffers(1, &positions_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, positions_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(),
                     positions.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Configure vertex array to use the position data
        glBindVertexArray(m.get_vao());
        glBindBuffer(GL_ARRAY_BUFFER, positions_vbo);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                              (void*)0);
		glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribDivisor(
            3, 1); // Only update data when drawing a new instance
        glBindVertexArray(0);
    }

private:
    friend class InstancedRenderer;
    Renderable* mesh;
    unsigned int positions_vbo;
};

//Deduction guide
template<typename Renderable>
InstancedMesh(Renderable& m, std::vector<glm::vec3> const& positions)
    ->InstancedMesh<Renderable>;

#endif
