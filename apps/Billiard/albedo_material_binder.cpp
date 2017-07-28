#include "albedo_material_binder.h"

AlbedoMaterialBinder::AlbedoMaterialBinder()
: shader_(nullptr)
{

}
void AlbedoMaterialBinder::Bind(sht::graphics::Material * material)
{
	if (shader_)
	{
		// Bind material to shader
		shader_->Uniform3fv("u_material.ambient", material->ambient);
		// Diffuse term is taken via albedo texture sampling
		shader_->Uniform3fv("u_material.specular", material->specular);
		shader_->Uniform3fv("u_material.emissive", material->emission);
		shader_->Uniform1f("u_material.shininess", material->shininess);
		shader_->Uniform1f("u_material.dissolve", material->dissolve);
	}
}
void AlbedoMaterialBinder::SetShader(sht::graphics::Shader * shader)
{
	shader_ = shader;
}