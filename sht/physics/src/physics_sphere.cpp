#include "physics_sphere.h"

#include <btBulletCollisionCommon.h>

namespace sht {
	namespace physics {

		Sphere::Sphere(const math::Vector3& position, float mass, float radius)
		: Object(position, math::Vector3(radius))
		, mass_(mass)
		, radius_(radius)
		{
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