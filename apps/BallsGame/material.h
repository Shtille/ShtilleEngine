#pragma once
#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <string>

namespace sht {
	namespace graphics {
		class Renderer;
		class Shader;
		class Texture;
	}
}

class Material {
public:
	Material(sht::graphics::Renderer * renderer,
		const std::string& material_name, const std::string& shader_name, const std::string& texture_name);
	virtual ~Material();

	virtual void Bind() = 0;

	virtual void GroupBind();
	virtual void GroupUnbind();

	sht::graphics::Shader * shader();
	sht::graphics::Texture * texture();

protected:
	sht::graphics::Renderer * renderer_;
	sht::graphics::Shader * shader_;
	sht::graphics::Texture * texture_;
	std::string material_name_;
	std::string shader_name_;
	std::string texture_name_;
};

#endif