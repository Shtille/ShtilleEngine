#pragma once
#ifndef __SHT_RENDERABLE_H__
#define __SHT_RENDERABLE_H__

namespace sht {
	namespace graphics {
		class Renderer;
		class Shader;
		class Model;
	}
}

namespace sht {

	class Renderable
	{
	public:
		explicit Renderable(graphics::Renderer * renderer, graphics::Shader * shader, graphics::Model * model);
		virtual ~Renderable();

		//! The main function that should be overrided by inheritants
		virtual void Render() = 0;

	protected:
		graphics::Renderer * renderer_;
		graphics::Shader * shader_;
		graphics::Model * model_;
	};

} // namespace sht

#endif