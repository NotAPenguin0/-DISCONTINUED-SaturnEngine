#ifndef MVG_INSTANCED_RENDERER_HPP_
#define MVG_INSTANCED_RENDERER_HPP_

#include "AbstractRenderer.hpp"
#include "InstancedMesh.hpp"
#include "Renderable.hpp"

class InstancedRenderer : public AbstractRenderer {
public:
	template<typename R>
	void render(InstancedMesh<R>& mesh, RenderTarget& target, std::size_t count)
	{
		static_assert(std::is_base_of_v<Renderable, R>, "Type must be a Renderable");
        target.set_as_active();
        active_shader->use();
		//Render using instancing
        mesh.mesh->render(*active_shader, true, count);
	}

private:
};

#endif
