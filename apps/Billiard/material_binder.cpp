#include "material_binder.h"

MaterialBinder::MaterialBinder()
: shader_(nullptr)
{

}
void MaterialBinder::Bind(sht::graphics::Material * material)
{
	if (shader_)
	{
		// Bind material to shader
		shader_->Uniform3fv("u_material.ambient", material->ambient);
		shader_->Uniform3fv("u_material.diffuse", material->diffuse);
		shader_->Uniform3fv("u_material.specular", material->specular);
		shader_->Uniform3fv("u_material.emissive", material->emission);
		shader_->Uniform1f("u_material.shininess", material->shininess);
		shader_->Uniform1f("u_material.dissolve", material->dissolve);
	}
}
void MaterialBinder::SetShader(sht::graphics::Shader * shader)
{
	shader_ = shader;
}