#include "ResourceLoaders.hpp"
#include "depend/stb_image.h"
#include "log.hpp"

#include <array>

namespace Saturn {

bool ResourceLoaders::loadModel(std::string path, Resources::Model& model) {
    using namespace std::literals::string_literals;

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
    // [Amount of vertices]
    // [The used attributes, space separated. This is a list containing any of
    // the supported attributes, which are listed below]
    // The list has to be in the order specified here
    // Attributes: position
    //			   color
    //			   normal
    //			   texcoord
    //"data" (without quotes, this one is important, the model will fail to load if you omit this)
    //[All vertex data, space separated with newlines where wanted]

    // Set attribute metadata and supported attributes
    static std::array supported_attributes{"position"s, "color"s, "normal"s,
                                           "texcoord"s};

    struct AttributeMetaData {
        std::size_t size;
        std::size_t location;
    };

#define DECLARE_ATTRIBUTE(name, size, location)                        \
    static AttributeMetaData name{size, location}

    DECLARE_ATTRIBUTE(position, 3, 0);
    DECLARE_ATTRIBUTE(color, 3, 1);
    DECLARE_ATTRIBUTE(normal, 3, 2);
    DECLARE_ATTRIBUTE(texcoord, 2, 3);

#undef DECLARE_ATTRIBUTE

    auto get_metadata = [](std::string_view name) {
        if (name == "position")
            return position;
        else if (name == "color")
            return color;
        else if (name == "normal")
            return normal;
        else if (name == "texcoord")
            return texcoord;
        else
            return AttributeMetaData{0, 0};
    };

    std::ifstream file(path);
    if (!file.good()) return false;

    int vertex_count;
    file >> vertex_count;

    std::vector<std::string> attributes;
    std::string attr;
    while (file >> attr) {
        if (attr == "data") break;
        if (std::find(supported_attributes.begin(), supported_attributes.end(),
                      attr) != supported_attributes.end()) {
            attributes.push_back(attr);
        } else {
            Saturn::error("Unknown attribute: "s + attr);
            return false;
        }
    }

    // Now, load in the vertex data
    std::vector<float> vertices;
    float elem;
    while (file >> elem) { vertices.push_back(elem); }

    // Now, fill the buffer
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                 vertices.data(), GL_STATIC_DRAW);

    std::size_t totalSize = vertices.size() / vertex_count;
    std::size_t offset = 0;
    // Set attribute pointers
    for (auto const& attr : attributes) {
        auto metadata = get_metadata(attr);
        glVertexAttribPointer(metadata.location, metadata.size, GL_FLOAT,
                              GL_FALSE, totalSize * sizeof(float),
                              (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(metadata.location);
		offset += metadata.size;
    }

    // Unbind the buffer and VAO object
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    model.vertex_count = vertex_count;

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