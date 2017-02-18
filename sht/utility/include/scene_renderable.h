#pragma once
#ifndef __SHT_UTILITY_SCENE_RENDERABLE_H__
#define __SHT_UTILITY_SCENE_RENDERABLE_H__

#include "scene_node.h"

// Forward declarations
namespace sht {
	namespace graphics {
		class Renderer;
		class Shader;
		class Model;
	}
}

namespace sht {
	namespace utility {

		class SceneRenderable : public SceneNode {
		public:
			explicit SceneRenderable(graphics::Renderer * renderer, graphics::Shader * shader, graphics::Model * model);
			virtual ~SceneRenderable();

		protected:
			graphics::Renderer * renderer_;
			graphics::Shader * shader_;
			graphics::Model * model_;
		};

	} // namespace utility
} // namespace sht

#endif