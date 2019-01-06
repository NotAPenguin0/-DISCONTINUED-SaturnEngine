#ifndef MVG_RENDER_TARGET_HPP_
#define MVG_RENDER_TARGET_HPP_

#include "OpenGL.hpp"
#include <GLM\glm.hpp>

class RenderTarget {
public:
    enum class Type { FrameBuffer, Screen };

    RenderTarget(Type t);
    virtual ~RenderTarget() = default;

    virtual void set_as_active() = 0;
    void clear(glm::vec3 color, GLenum buffers);

    Type get_type() const;

private:
    Type type;
};

#endif
