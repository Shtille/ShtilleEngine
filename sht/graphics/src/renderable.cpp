#include "../include/renderable.h"

namespace sht {
	namespace graphics {

		Renderable::Renderable(Renderer * renderer, Shader * shader, Model * model)
		: renderer_(renderer)
		, shader_(shader)
		, model_(model)
		{
		}
		Renderable::~Renderable()
		{
		}

	} // namespace graphics
} // namespace sht