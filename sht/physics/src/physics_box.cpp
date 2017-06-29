#include "physics_box.h"

#include "../include/physics_unit_converter.h"

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

		}
		Box::~Box()
		{
		}
		void Box::CreateShape(const UnitConversion * unit_conversion)
		{
			unit_conversion_ = unit_conversion;
			if (unit_conversion_ && unit_conversion_->linear_to)
			{
				unit_conversion_->linear_to(&size_x_);
				unit_conversion_->linear_to(&size_y_);
				unit_conversion_->linear_to(&size_z_);
			}
			shape_ = new btBoxShape(btVector3(size_x_, size_y_, size_z_));
		}
		float Box::mass() const
		{
			return mass_;
		}

	} // namespace physics
} // namespace sht