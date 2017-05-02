#include "object_manager.h"

#include "material.h"

#include "physics/include/physics_engine.h"
#include "utility/include/event.h"

ObjectManager::ObjectManager(sht::graphics::Renderer * renderer, sht::physics::Engine * physics_engine,
	const vec3* light_pos_eye, sht::utility::EventListenerInterface * event_listener)
: renderer_(renderer)
, physics_engine_(physics_engine)
, event_listener_(event_listener)
, shader_db_(renderer)
, texture_db_(renderer)
, surface_db_()
, model_db_(renderer)
, material_db_(renderer, &shader_db_, &texture_db_, light_pos_eye)
, root_node_()
, last_object_(nullptr)
, need_sort_(false)
, editor_mode_(false)
{

}
ObjectManager::~ObjectManager()
{
}
sht::SimpleObject * ObjectManager::last_object()
{
	return last_object_;
}
void ObjectManager::set_editor_mode(bool editor_mode)
{
	editor_mode_ = editor_mode;

	sht::utility::Event event(SID("editor_mode_changed"));
	event_listener_->OnEvent(&event);
}
bool ObjectManager::editor_mode() const
{
	return editor_mode_;
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
void ObjectManager::AddSphere(float pos_x, float pos_y, float pos_z, float mass, float radius,
		std::string material_name)
{
	const char * kModelName = "sphere";

	Material * material = material_db_.Get(material_name);
	Surface * surface = surface_db_.Get(material_name);
	sht::graphics::Model * model = model_db_.Get(kModelName);

	sht::physics::Object * object;
	sht::math::Vector3 position(pos_x, pos_y, pos_z);
    object = physics_engine_->AddSphere(position, mass, radius);
    object->SetFriction(surface->linear_friction);
    object->SetRollingFriction(surface->rolling_friction);
    object->SetSpinningFriction(surface->spinning_friction);
    object->SetRestitution(surface->restitution);

    sht::SimpleObject * simple_object;
    simple_object = new sht::SimpleObject(renderer_, material->shader(), model, object, material);
    last_object_ = simple_object;
    root_node_.Attach(simple_object);

    need_sort_ = true;
}
void ObjectManager::AddBox(float pos_x, float pos_y, float pos_z, float mass, float size_x, float size_y, float size_z,
		std::string material_name)
{
	const char * kModelName = "box";

	Material * material = material_db_.Get(material_name);
	Surface * surface = surface_db_.Get(material_name);
	sht::graphics::Model * model = model_db_.Get(kModelName);

	sht::physics::Object * object;
	sht::math::Vector3 position(pos_x, pos_y, pos_z);
    object = physics_engine_->AddBox(position, mass, size_x, size_y, size_z);
    object->SetFriction(surface->linear_friction);
    object->SetRollingFriction(surface->rolling_friction);
    object->SetSpinningFriction(surface->spinning_friction);
    object->SetRestitution(surface->restitution);

    sht::SimpleObject * simple_object;
    simple_object = new sht::SimpleObject(renderer_, material->shader(), model, object, material);
    last_object_ = simple_object;
    root_node_.Attach(simple_object);

    need_sort_ = true;
}