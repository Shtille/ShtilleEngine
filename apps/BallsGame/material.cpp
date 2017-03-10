#include "material.h"

Material::Material(sht::graphics::Renderer * renderer,
		const std::string& material_name, const std::string& shader_name, const std::string& texture_name)
	: renderer_(renderer)
	, shader_(nullptr)
	, texture_(nullptr)
	, material_name_(material_name)
	, shader_name_(shader_name)
	, texture_name_(texture_name)
{

}
Material::~Material()
{

}
void Material::GroupBind()
{

}
void Material::GroupUnbind()
{
	
}
sht::graphics::Shader * Material::shader()
{
	return shader_;
}
sht::graphics::Texture * Material::texture()
{
	return texture_;
}