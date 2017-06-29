#pragma once
#ifndef __SHT_PHYSICS_ENGINE_H__
#define __SHT_PHYSICS_ENGINE_H__

#include "physics_unit_converter.h"

#include "graphics/include/model/mesh_vertices_enumerator.h"

#include "math/vector.h"
#include "math/matrix.h"

#include <vector>

// Forward Bullet declarations
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

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

			void SetLinearToFunction(ConversionFunction function);
			void SetLinearFromFunction(ConversionFunction function);
			void SetMassToFunction(ConversionFunction function);
			void SetMassFromFunction(ConversionFunction function);

			// We can ignore return value if we don't render this object
			Object * AddSphere(const math::Vector3& position,
				float mass, float radius);
			Object * AddBox(const math::Vector3& position,
				float mass, float size_x, float size_y, float size_z);
			Object * AddMesh(const math::Vector3& position,
				float mass, graphics::MeshVerticesEnumerator * enumerator);

		protected:
			void ReleaseObjects();
			void AddCustomObject(Object * object, float mass);
			
		private:
			UnitConversion unit_conversion_;
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