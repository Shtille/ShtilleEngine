#pragma once
#ifndef __SHT_PHYSICS_MOTION_STATE_H__
#define __SHT_PHYSICS_MOTION_STATE_H__

#include "math/vector.h"
#include "math/matrix.h"

#include <LinearMath/btMotionState.h>

namespace sht {
	namespace physics {

		class MotionState : public btMotionState
		{
		public:
			explicit MotionState(const btTransform& transform);
			virtual ~MotionState();

			virtual void getWorldTransform(btTransform &world_transform) const;
			virtual void setWorldTransform(const btTransform &world_transform);

			void GetPosition(math::Vector3 * position);
			
		private:
			btTransform transform_;
		};

	} // namespace physics
} // namespace sht

#endif