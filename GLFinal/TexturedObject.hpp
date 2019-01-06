#ifndef MVG_TEXTURED_OBJECT_HPP_
#define MVG_TEXTURED_OBJECT_HPP_

#include "Mesh.hpp"
#include "Renderable.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

// Class to link texture and mesh together
class TexturedObject : public Renderable {
public:
    TexturedObject(Mesh& m, Saturn::Texture& tex);

    void render(Shader& shader,
                bool instanced = false,
                std::size_t count = 0) override;

    inline unsigned int get_vao() override { return mesh->get_vao(); }

private:
    Saturn::Texture* texture;
};

#endif
