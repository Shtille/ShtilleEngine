#include "../include/physics_engine.h"

#include "physics_sphere.h"
#include "physics_box.h"
#include "physics_mesh.h"

#include <btBulletDynamicsCommon.h>

namespace sht {
	namespace physics {

		Engine::Engine(const UnitConverter * unit_converter)
		: unit_converter_(unit_converter)
		{
			collision_configuration_ = new btDefaultCollisionConfiguration();
			dispatcher_ = new btCollisionDispatcher(collision_configuration_);
			broadphase_ = new btDbvtBroadphase();
			solver_ = new btSequentialImpulseConstraintSolver();
			dynamics_world_ = new btDiscreteDynamicsWorld(dispatcher_, broadphase_, solver_, collision_configuration_);
		}
		Engine::~Engine()
		{
			ReleaseObjects();
			delete dynamics_world_;
			delete solver_;
			delete broadphase_;
			delete dispatcher_;
			delete collision_configuration_;
		}
		void Engine::SetGravity(const math::Vector3& acceleration)
		{
			dynamics_world_->setGravity(btVector3(acceleration.x, acceleration.y, acceleration.z));
		}
		Object * Engine::AddSphere(const math::Vector3& position,
				float mass, float radius)
		{
			Sphere * object = new Sphere(position, mass, radius, unit_converter_);
			AddCustomObject(object, mass);
			return object;
		}
		Object * Engine::AddBox(const math::Vector3& position,
				float mass, float size_x, float size_y, float size_z)
		{
			Box * object = new Box(position, mass, size_x, size_y, size_z, unit_converter_);
			AddCustomObject(object, mass);
			return object;
		}
		Object * Engine::AddMesh(const math::Vector3& position,
				float mass, graphics::MeshVerticesEnumerator * enumerator)
		{
			Mesh * object = new Mesh(position, mass, enumerator, unit_converter_);
			AddCustomObject(object, mass);
			return object;
		}
		void Engine::ReleaseObjects()
		{
			for (auto object : objects_)
			{
				delete object;
			}
		}
		void Engine::AddCustomObject(Object * object, float mass)
		{
			btCollisionShape * shape = object->shape_;
			btVector3 local_inertia(0.0f, 0.0f, 0.0f);
			if (mass != 0.0f)
				shape->calculateLocalInertia(mass, local_inertia);
			btRigidBody::btRigidBodyConstructionInfo rb_info(mass, object, shape, local_inertia);
			btRigidBody * body = new btRigidBody(rb_info);
			dynamics_world_->addRigidBody(body);
			object->ApplyScale();
			object->body_ = body;
			// Finally
			objects_.push_back(object);
		}
		void Engine::Update(float sec)
		{
			const int kMaxSubSteps = 10;
			const float kFixedTimeStep = 0.01f;
			dynamics_world_->stepSimulation(sec, kMaxSubSteps, kFixedTimeStep);
		}

	} // namespace physics
} // namespace sht