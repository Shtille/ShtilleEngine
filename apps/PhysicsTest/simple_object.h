#pragma once
#ifndef __SHT_SIMPLE_OBJECT_H__
#define __SHT_SIMPLE_OBJECT_H__

#include "renderable.h"
#include "physics_object.h"

namespace sht {

	class SimpleObject : public Renderable
	{
	public:
		explicit SimpleObject(graphics::Renderer * renderer, graphics::Shader * shader, graphics::Model * model,
			physics::Object * physics_object);
		virtual ~SimpleObject();

		void Render() override;

		//! Returns physics object to let user to apply some forces/torques onto it
		physics::Object * body();

	protected:
		physics::Object * physics_object_;
	};

} // namespace sht

#endif