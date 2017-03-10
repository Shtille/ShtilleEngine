#include "object_manager.h"

#include "material.h"

#include "physics/include/physics_engine.h"

ObjectManager::ObjectManager(sht::graphics::Renderer * renderer, sht::physics::Engine * physics_engine,
	const vec3* light_pos_eye)
: renderer_(renderer)
, physics_engine_(physics_engine)
, shader_db_(renderer)
, texture_db_(renderer)
, surface_db_()
, model_db_(renderer)
, material_db_(renderer, &shader_db_, &texture_db_, light_pos_eye)
, root_node_()
, need_sort_(false)
{

}
ObjectManager::~ObjectManager()
{
}
void ObjectManager::RenderAll()
{
	if (need_sort_)
	{
		root_node_.Sort(
			[](sht::utility::SceneNode * n1, sht::utility::SceneNode * n2) -> bool
			{
				return n1->hash() < n2->hash();
			});
	}
	// All the objects with equal shader should be unioned into groups
	size_t num_children = root_node_.num_children();
	for (size_t i = 0; i < num_children; ++i)
	{
		sht::SimpleObject * child = dynamic_cast<sht::SimpleObject*>(root_node_.child(i));
		sht::SimpleObject * next_child = (i + 1 != num_children) ? 
			(dynamic_cast<sht::SimpleObject*>(root_node_.child(i + 1))) : (nullptr);
		if (i == 0)
		{
			child->material()->GroupBind();
		}
		child->Render();
		if (next_child && next_child->hash() != child->hash())
		{
			child->material()->GroupUnbind();
			next_child->material()->GroupBind();
		}
		else if (!next_child)
		{
			child->material()->GroupUnbind();
		}
	}
}
sht::SimpleObject * ObjectManager::AddSphere(const vec3& position, float mass, float radius,
		const std::string& material_name)
{
	const char * kModelName = "sphere";

	Material * material = material_db_.Get(material_name);
	Surface * surface = surface_db_.Get(material_name);
	sht::graphics::Model * model = model_db_.Get(kModelName);

	sht::physics::Object * object;
    object = physics_engine_->AddSphere(position, mass, radius);
    object->SetFriction(surface->linear_friction);
    object->SetRollingFriction(surface->rolling_friction);
    object->SetSpinningFriction(surface->spinning_friction);
    object->SetRestitution(surface->restitution);

    sht::SimpleObject * simple_object;
    simple_object = new sht::SimpleObject(renderer_, material->shader(), model, object, material);

    root_node_.Attach(simple_object);
    need_sort_ = true;
    return simple_object;
}
sht::SimpleObject * ObjectManager::AddBox(const vec3& position, float mass, float size_x, float size_y, float size_z,
		const std::string& material_name)
{
	const char * kModelName = "box";

	Material * material = material_db_.Get(material_name);
	Surface * surface = surface_db_.Get(material_name);
	sht::graphics::Model * model = model_db_.Get(kModelName);

	sht::physics::Object * object;
    object = physics_engine_->AddBox(position, mass, size_x, size_y, size_z);
    object->SetFriction(surface->linear_friction);
    object->SetRollingFriction(surface->rolling_friction);
    object->SetSpinningFriction(surface->spinning_friction);
    object->SetRestitution(surface->restitution);

    sht::SimpleObject * simple_object;
    simple_object = new sht::SimpleObject(renderer_, material->shader(), model, object, material);

    root_node_.Attach(simple_object);
    need_sort_ = true;
    return simple_object;
}