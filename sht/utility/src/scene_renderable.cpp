#include "../include/scene_renderable.h"

namespace sht {
	namespace utility {

		SceneRenderable::SceneRenderable(graphics::Renderer * renderer, graphics::Shader * shader, graphics::Model * model)
		: SceneNode()
		, renderer_(renderer)
		, shader_(shader)
		, model_(model)
		{

		}
		SceneRenderable::~SceneRenderable()
		{

		}

	} // namespace utility
} // namespace sht