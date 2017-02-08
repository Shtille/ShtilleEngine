#include "renderable.h"

namespace sht {

	Renderable::Renderable(graphics::Renderer * renderer, graphics::Shader * shader, graphics::Model * model)
	: renderer_(renderer)
	, shader_(shader)
	, model_(model)
	{
	}
	Renderable::~Renderable()
	{
	}

} // namespace sht