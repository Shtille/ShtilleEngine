#include "physics_motion_state.h"

namespace sht {
	namespace physics {

		MotionState::MotionState(const btTransform& transform)
		: transform_(transform)
		{

		}
		MotionState::~MotionState()
		{

		}
		void MotionState::getWorldTransform(btTransform &world_transform) const
		{
			world_transform = transform_;
		}
		void MotionState::setWorldTransform(const btTransform &world_transform)
		{
			transform_ = world_transform;
		}
		void MotionState::GetPosition(math::Vector3 * position)
		{
			btVector3 pos = transform_.getOrigin();
			position->x = pos.x();
			position->y = pos.y();
			position->z = pos.z();
		}

	} // namespace physics
} // namespace sht