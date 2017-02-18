#pragma once
#ifndef __SHT_PHYSICS_SPHERE_H__
#define __SHT_PHYSICS_SPHERE_H__

#include "../include/physics_object.h"

namespace sht {
	namespace physics {

		class Sphere : public Object
		{
		public:
			explicit Sphere(const math::Vector3& position, float mass, float radius);
			virtual ~Sphere();

			float mass() const;
			float radius() const;

		protected:
			float mass_;
			float radius_;
		};

	} // namespace physics
} // namespace sht

#endif