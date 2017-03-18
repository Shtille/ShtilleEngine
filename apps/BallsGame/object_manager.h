#pragma once
#ifndef __OBJECT_MANAGER_H__
#define __OBJECT_MANAGER_H__

#include "shader_database.h"
#include "texture_database.h"
#include "surface_database.h"
#include "model_database.h"
#include "material_database.h"

#include "simple_object.h"

#include <string>

namespace sht {
	namespace physics {
		class Engine;
	}
}

class ObjectManager {
public:
	ObjectManager(sht::graphics::Renderer * renderer, sht::physics::Engine * physics_engine, const vec3* light_pos_eye);
	~ObjectManager();

	sht::SimpleObject * last_object();

	void set_editor_mode(bool editor_mode);
	void reset_editor_mode();
	bool editor_mode();
	bool editor_mode_changed();

	void RenderAll();

	void AddSphere(float pos_x, float pos_y, float pos_z, float mass, float radius,
		std::string material_name);
	void AddBox(float pos_x, float pos_y, float pos_z, float mass, float size_x, float size_y, float size_z,
		std::string material_name);

private:
	sht::graphics::Renderer * renderer_;
	sht::physics::Engine * physics_engine_;
	ShaderDatabase shader_db_;
	TextureDatabase texture_db_;
	SurfaceDatabase surface_db_;
	ModelDatabase model_db_;
	MaterialDatabase material_db_;
	sht::utility::SceneNode root_node_;
	sht::SimpleObject * last_object_;
	bool need_sort_;
	bool editor_mode_;
	bool editor_mode_changed_;
};

#endif