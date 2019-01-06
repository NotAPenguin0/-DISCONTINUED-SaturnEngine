#include "Texture.hpp"

#include "depend/stb_image.h"
#include "log.hpp"

#include <string>

namespace Saturn {

bool Texture::load(const char* fname,
                   int unit /* = GL_TEXTURE0 */,
                   int format /* = GL_RGB */,
                   bool flip /*= false*/) {
    using namespace std::literals::string_literals;

    if (handle() != 0) glDeleteTextures(1, &texture_handle);

    texture_handle = -1;
    texture_unit = unit;

	stbi_set_flip_vertically_on_load(flip);

    int width, height, channels;
    int32_t desired_channel_count = 4;
    unsigned char* image_data =
        stbi_load(fname, &width, &height, &channels, desired_channel_count);
    if (!image_data) { throw std::runtime_error("Image not loaded"); }
    glGenTextures(1, &texture_handle);
    glBindTexture(GL_TEXTURE_2D, texture_handle);

    if (format != GL_RGBA) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // Set min and mag filters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, image_data);

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image_data);

    return true;
}

Texture::~Texture() {
    if (handle() != 0) glDeleteTextures(1, &texture_handle);
}

void Texture::bind() const {
    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_2D, texture_handle);
}

void Texture::unbind() const {
    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int Texture::handle() const { return texture_handle; }

int Texture::unit() const { return texture_unit; }
int Texture::height() const { return h; }
int Texture::width() const { return w; }

void Texture::set_parameter(int param, int val) {
    bind();
    glTexParameteri(GL_TEXTURE_2D, param, val);
}

} // namespace Saturn
