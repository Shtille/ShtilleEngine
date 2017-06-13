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
	}
}
void MaterialBinder::SetShader(sht::graphics::Shader * shader)
{
	shader_ = shader;
}