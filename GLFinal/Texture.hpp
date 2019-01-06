#ifndef SATURN_TEXTURE_HPP_
#define SATURN_TEXTURE_HPP_

#include "glad/glad.h"

namespace Saturn {

class Texture {
public:
    Texture() = default;
    Texture(Texture const&) = delete;
    Texture& operator=(Texture const&) = delete;

	~Texture();

    bool load(const char* fname, int unit = GL_TEXTURE0, int format = GL_RGB, bool flip = false);
	
	void bind() const;
    void unbind() const;

	unsigned int handle() const;
    int unit() const;
    int width() const;
    int height() const;

	void set_parameter(int param, int val);

private:
    unsigned int texture_handle;
    int texture_unit;
    int w, h;
};

} // namespace Saturn

#endif