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
		size_t SceneRenderable::hash()
		{
			return reinterpret_cast<size_t>(shader_);
		}
		sht::graphics::Shader * SceneRenderable::shader()
		{
			return shader_;
		}
		sht::graphics::Model * SceneRenderable::model()
		{
			return model_;
		}

	} // namespace utility
} // namespace sht