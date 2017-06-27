#include "physics_box.h"

#include "../include/physics_unit_converter.h"

#include <btBulletCollisionCommon.h>

namespace sht {
	namespace physics {

		Box::Box(const math::Vector3& position, float mass, float size_x, float size_y, float size_z,
			const UnitConverter * unit_converter)
		: Object(position)
		, mass_(mass)
		, size_x_(size_x)
		, size_y_(size_y)
		, size_z_(size_z)
		{
			unit_converter_ = unit_converter;
			if (unit_converter)
			{
				unit_converter->LinearScaleToStandard(&size_x_);
				unit_converter->LinearScaleToStandard(&size_y_);
				unit_converter->LinearScaleToStandard(&size_z_);
			}
			shape_ = new btBoxShape(btVector3(size_x_, size_y_, size_z_));
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