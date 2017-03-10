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

	void RenderAll();

	sht::SimpleObject * AddSphere(const vec3& position, float mass, float radius,
		const std::string& material_name);
	sht::SimpleObject * AddBox(const vec3& position, float mass, float size_x, float size_y, float size_z,
		const std::string& material_name);

private:
	sht::graphics::Renderer * renderer_;
	sht::physics::Engine * physics_engine_;
	ShaderDatabase shader_db_;
	TextureDatabase texture_db_;
	SurfaceDatabase surface_db_;
	ModelDatabase model_db_;
	MaterialDatabase material_db_;
	sht::utility::SceneNode root_node_;
	bool need_sort_;
};

#endif