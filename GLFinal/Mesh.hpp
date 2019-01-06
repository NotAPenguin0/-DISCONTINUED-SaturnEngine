#ifndef ENGINE_MESH_HPP_INCLUDE
#define ENGINE_MESH_HPP_INCLUDE

#include <GLM/glm.hpp>
#include <string>
#include <vector>

#include "Shader.hpp"

enum class Texture_type {
    specular,
    diffuse,
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv_coordinates;

    inline Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 uv) :
        position(p), normal(n), uv_coordinates(uv) {}
    inline Vertex() {}
    Vertex(Vertex const&) = default;
    Vertex(Vertex&&) = default;
    Vertex& operator=(Vertex const&) = default;
    Vertex& operator=(Vertex&&) = default;

    ~Vertex() = default;
};

struct Texture {
    std::string name;
    Texture_type type;
    uint32_t id;
};

class Mesh {
public:
    Mesh();
    Mesh(std::vector<Vertex> const& vertices,
         std::vector<uint32_t> const& indices,
         std::vector<Texture> const& textures);
    Mesh(std::vector<Vertex>&& vertices,
         std::vector<uint32_t>&& indices,
         std::vector<Texture>&& textures);
    Mesh(Mesh&&);
    Mesh& operator=(Mesh&&);
    virtual ~Mesh();

    Mesh(Mesh const&) = delete;
    Mesh& operator=(Mesh const&) = delete;

    virtual void render(Shader& shader, bool instanced = false, std::size_t count = 0);

    inline unsigned int get_vao() { return vao; }

protected:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture> textures;
    uint32_t vao = 0;
    uint32_t vbo = 0;
    uint32_t ebo = 0;

    void prepare_mesh();
};

#endif // !ENGINE_MESH_HPP_INCLUDE
