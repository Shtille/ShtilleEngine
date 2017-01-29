#pragma once
#ifndef __SHT_PHYSICS_LOGICS_H__
#define __SHT_PHYSICS_LOGICS_H__

#include "math/vector.h"
#include "math/matrix.h"

#include <btBulletDynamicsCommon.h>

namespace sht {
	namespace physics {

		class MotionState;

		class Logics
		{
		public:
			Logics();
			~Logics();

			void SetupObjects();

			void Update(float sec);

			void GetPosition(math::Vector3 * position);

		protected:
			void ReleaseObjects();
			
		private:
			btDefaultCollisionConfiguration * collision_configuration_;
			btCollisionDispatcher * dispatcher_;
			btBroadphaseInterface * broadphase_;
			btSequentialImpulseConstraintSolver * solver_;
			btDiscreteDynamicsWorld * dynamics_world_;
			btAlignedObjectArray<btCollisionShape*> collision_shapes_;

			MotionState * object_motion_state_;
		};

	} // namespace physics
} // namespace sht

#endif