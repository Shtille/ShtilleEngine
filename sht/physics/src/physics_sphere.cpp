#include "physics_sphere.h"

#include "../include/physics_unit_converter.h"

#include <btBulletCollisionCommon.h>

namespace sht {
	namespace physics {

		Sphere::Sphere(const math::Vector3& position, float mass, float radius)
		: Object(position)
		, mass_(mass)
		, radius_(radius)
		{
			
		}
		Sphere::~Sphere()
		{
		}
		void Sphere::CreateShape(const UnitConversion * unit_conversion)
		{
			unit_conversion_ = unit_conversion;
			if (unit_conversion_ && unit_conversion_->linear_to)
			{
				unit_conversion_->linear_to(&radius_);
			}
			shape_ = new btSphereShape(radius_);
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