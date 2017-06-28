#include "physics_box.h"

#include <btBulletCollisionCommon.h>

namespace sht {
	namespace physics {

		Box::Box(const math::Vector3& position, float mass, float size_x, float size_y, float size_z)
		: Object(position)
		, mass_(mass)
		, size_x_(size_x)
		, size_y_(size_y)
		, size_z_(size_z)
		{
			shape_ = new btBoxShape(btVector3(size_x, size_y, size_z));
		}
		Box::~Box()
		{
		}
		float Box::mass() const
		{
			return mass_;
		}

	} // namespace physics
} // namespace sht