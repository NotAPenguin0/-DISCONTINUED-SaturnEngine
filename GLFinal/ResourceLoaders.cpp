#include "ResourceLoaders.hpp"
#include "depend/stb_image.h"
#include "log.hpp"

namespace Saturn {

bool ResourceLoaders::loadModel(std::string path, Resources::Model& model) {
    // Aliases to make the code a bit more readable
    auto& vao = model.vao;
    auto& vbo = model.vbo;

    // Generate the buffers
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    // Bind the buffers
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Now we load the vertex data from the file
    // A vertex data file looks like this:
    // Vertex array data, comma separated

    std::ifstream file(path);
    if (!file.good()) return false;

    std::vector<float> vertices;
    float elem;
    while (file >> elem) { vertices.push_back(elem); }

    // Now, fill the buffer
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                 vertices.data(), GL_STATIC_DRAW);
    // Currently we only support positions. This will change in the future
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);
    // Texture Coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind the buffer and VAO object
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    model.vertex_count = vertices.size() / 5;

    return true;
}

void ResourceLoaders::unloadModel(Resources::Model& model) {
    glDeleteVertexArrays(1, &model.vao);
    glDeleteBuffers(1, &model.vbo);
}

bool ResourceLoaders::loadTexture(std::string path,
                                  Resources::Texture& texture) {
    // A texture file first has the texture unit, and then the
    // path to the actual image

    std::ifstream file(path);
    if (!file.good()) return false;

    int unit;
    file >> unit;
    unit += GL_TEXTURE0; // Make it relative to GL_TEXTURE0
    texture.unit = unit; // Now set the unit value

    std::string imgPath;
    file >> imgPath;
    file.close();

    stbi_set_flip_vertically_on_load(true);

    int channels;
    unsigned char* data =
        stbi_load(path.c_str(), &texture.width, &texture.height, &channels, 0);
    if (data == nullptr) {
        Saturn::error("Failed to load texture: " + path);
        return false;
    }

    GLenum format = GL_RGB; // default, might not always work
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;

    // Generate texture handle
    glGenTextures(1, &texture.handle);
    glActiveTexture(texture.unit);
    glBindTexture(GL_TEXTURE_2D, texture.handle);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Send image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, format, texture.width, texture.height, 0,
                 format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return true;
}

void ResourceLoaders::unloadTexture(Resources::Texture& texture) {
    glDeleteTextures(1, &texture.handle);
}

} // namespace Saturn
