#pragma once
#ifndef __SHT_PHYSICS_BOX_H__
#define __SHT_PHYSICS_BOX_H__

#include "physics_object.h"

namespace sht {
	namespace physics {

		class Box : public Object
		{
		public:
			explicit Box(const math::Vector3& position, float mass, float size_x, float size_y, float size_z);
			virtual ~Box();

			float mass() const;

		protected:
			float mass_;
			float size_x_;
			float size_y_;
			float size_z_;
		};

	} // namespace physics
} // namespace sht

#endif