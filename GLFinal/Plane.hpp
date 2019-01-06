#ifndef MVG_PLANE_HPP_
#define MVG_PLANE_HPP_

#include "Mesh.hpp"

class Plane : public Mesh
{
public:
    Plane(float scale = 1.0f);

	virtual void render(Shader& shader, bool instanced = false, std::size_t count = 0) override;
};

#endif