#ifndef MVG_RENDERABLE_HPP_
#define MVG_RENDERABLE_HPP_

class Mesh;
class Shader;

#include <cstddef>

// Class that satisfies the requirements of Renderable
class Renderable {
public:
    virtual unsigned int get_vao() = 0;
    virtual void
    render(Shader& shader, bool instanced = false, std::size_t count = 0) = 0;

protected:
    Mesh* mesh;
};

#endif
