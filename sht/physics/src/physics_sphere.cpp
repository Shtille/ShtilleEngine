#include "physics_sphere.h"

#include "../include/physics_unit_converter.h"

#include <btBulletCollisionCommon.h>

namespace sht {
	namespace physics {

		Sphere::Sphere(const math::Vector3& position, float mass, float radius,
			const UnitConverter * unit_converter)
		: Object(position)
		, mass_(mass)
		, radius_(radius)
		{
			unit_converter_ = unit_converter;
			if (unit_converter)
			{
				unit_converter->LinearScaleToStandard(&radius_);
			}
			shape_ = new btSphereShape(radius_);
		}
		Sphere::~Sphere()
		{
		}
		float Sphere::mass() const
		{
			return mass_;
		}
		float Sphere::radius() const
		{
			return radius_;
		}

	} // namespace physics
} // namespace sht