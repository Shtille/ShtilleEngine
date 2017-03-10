#include "simple_material.h"

#include "shader_database.h"
#include "texture_database.h"

#include "graphics/include/renderer/renderer.h"

SimpleMaterial::SimpleMaterial(sht::graphics::Renderer * renderer, ShaderDatabase * shader_db, TextureDatabase * texture_db,
		const sht::math::Vector3 * light_pos_eye, const std::string& texture_name)
: Material(renderer, "simple_material", "object_shader", texture_name)
, shader_db_(shader_db)
, texture_db_(texture_db)
, light_pos_eye_(light_pos_eye)
{
	shader_ = shader_db_->Get(shader_name_);
	texture_ = texture_db_->Get(texture_name_);
}
SimpleMaterial::~SimpleMaterial()
{
	shader_db_->Release(shader_name_);
	texture_db_->Release(texture_name_);
}
void SimpleMaterial::Bind()
{
	sht::math::Matrix3 normal_matrix;
	renderer_->ChangeTexture(texture_, 0);
	shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
    normal_matrix = sht::math::NormalMatrix(renderer_->view_matrix() * renderer_->model_matrix());
    shader_->UniformMatrix3fv("u_normal_matrix", normal_matrix);
}
void SimpleMaterial::GroupBind()
{
	shader_->Bind();
    shader_->UniformMatrix4fv("u_projection", renderer_->projection_matrix());
    shader_->UniformMatrix4fv("u_view", renderer_->view_matrix());
    shader_->Uniform3fv("u_light_pos", *light_pos_eye_);
    shader_->Uniform1i("u_texture", 0);
}