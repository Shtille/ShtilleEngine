#pragma once
#ifndef __SHT_SIMPLE_OBJECT_H__
#define __SHT_SIMPLE_OBJECT_H__

#include "utility/include/scene_renderable.h"
#include "physics/include/physics_object.h"

class Material;

namespace sht {

	class SimpleObject : public utility::SceneRenderable
	{
	public:
		explicit SimpleObject(graphics::Renderer * renderer, graphics::Shader * shader, graphics::Model * model,
			physics::Object * physics_object, Material * material);
		virtual ~SimpleObject();

		virtual void Render() override;

		//! Returns physics object to let user to apply some forces/torques onto it
		physics::Object * body();
		Material * material();

	protected:
		physics::Object * physics_object_;
		Material * material_;
	};

} // namespace sht

#endif