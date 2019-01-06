#include "TexturedObject.hpp"

TexturedObject::TexturedObject(Mesh& m, Saturn::Texture& tex) {
    mesh = &m;
    texture	= &tex;
}

void TexturedObject::render(Shader& shader, bool instanced, std::size_t count) {
    shader.use();
    texture->bind();
    mesh->render(shader, instanced, count);
}