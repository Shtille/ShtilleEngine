#pragma once
#ifndef __SHT_GRAPHICS_RENDERABLE_H__
#define __SHT_GRAPHICS_RENDERABLE_H__

namespace sht {
	namespace graphics {
		class Renderer;
		class Shader;
		class Model;
	}
}

namespace sht {
	namespace graphics {

		class Renderable
		{
		public:
			explicit Renderable(Renderer * renderer, Shader * shader, Model * model);
			virtual ~Renderable();

			//! The main function that should be overrided by inheritants
			virtual void Render() = 0;

		protected:
			Renderer * renderer_;
			Shader * shader_;
			Model * model_;
		};

	} // namespace graphics 
} // namespace sht

#endif