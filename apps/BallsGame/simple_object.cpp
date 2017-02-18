#include "simple_object.h"

#include "graphics/include/renderer/renderer.h"
#include "graphics/include/model/model.h"

namespace sht {

	SimpleObject::SimpleObject(graphics::Renderer * renderer, graphics::Shader * shader, graphics::Model * model,
			physics::Object * physics_object)
	: utility::SceneRenderable(renderer, shader, model)
	, physics_object_(physics_object)
	{
	}
	SimpleObject::~SimpleObject()
	{
	}
	void SimpleObject::Render()
	{
		sht::math::Matrix3 normal_matrix;
		renderer_->PushMatrix();
        renderer_->MultMatrix(physics_object_->matrix());
        shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
        normal_matrix = sht::math::NormalMatrix(renderer_->view_matrix() * renderer_->model_matrix());
        shader_->UniformMatrix3fv("u_normal_matrix", normal_matrix);
        model_->Render();
        renderer_->PopMatrix();
	}
	physics::Object * SimpleObject::body()
	{
		return physics_object_;
	}

} // namespace sht