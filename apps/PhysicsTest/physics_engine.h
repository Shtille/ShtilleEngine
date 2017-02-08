#pragma once
#ifndef __SHT_PHYSICS_ENGINE_H__
#define __SHT_PHYSICS_ENGINE_H__

#include "math/vector.h"
#include "math/matrix.h"

#include <btBulletDynamicsCommon.h>

#include <vector>

namespace sht {
	namespace physics {

		class Object;

		class Engine
		{
		public:
			Engine();
			~Engine();

			void Update(float sec);

			void SetGravity(const math::Vector3& acceleration);

			// We can ignore return value if we don't render this object
			Object * AddSphere(const math::Vector3& position,
				float mass, float radius);
			Object * AddBox(const math::Vector3& position,
				float mass, float size_x, float size_y, float size_z);

		protected:
			void ReleaseObjects();
			void AddCustomObject(Object * object, float mass);
			
		private:
			btDefaultCollisionConfiguration * collision_configuration_;
			btCollisionDispatcher * dispatcher_;
			btBroadphaseInterface * broadphase_;
			btSequentialImpulseConstraintSolver * solver_;
			btDiscreteDynamicsWorld * dynamics_world_;

			std::vector<Object*> objects_;
		};

	} // namespace physics
} // namespace sht

#endif