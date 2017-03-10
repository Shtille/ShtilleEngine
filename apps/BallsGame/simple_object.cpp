#include "simple_object.h"
#include "material.h"

#include "graphics/include/renderer/renderer.h"
#include "graphics/include/model/model.h"

namespace sht {

	SimpleObject::SimpleObject(graphics::Renderer * renderer, graphics::Shader * shader, graphics::Model * model,
			physics::Object * physics_object, Material * material)
	: utility::SceneRenderable(renderer, shader, model)
	, physics_object_(physics_object)
	, material_(material)
	{
	}
	SimpleObject::~SimpleObject()
	{
	}
	void SimpleObject::Render()
	{
		renderer_->PushMatrix();
        renderer_->MultMatrix(physics_object_->matrix());
        material_->Bind();
        model_->Render();
        renderer_->PopMatrix();
	}
	physics::Object * SimpleObject::body()
	{
		return physics_object_;
	}
	Material * SimpleObject::material()
	{
		return material_;
	}

} // namespace sht