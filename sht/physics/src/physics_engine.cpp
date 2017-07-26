#include "../include/physics_engine.h"
#include "../include/physics_ghost_object.h"

#include "physics_sphere.h"
#include "physics_box.h"
#include "physics_mesh.h"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include <cstring>

namespace {
	class ContactSensorCallback : public btCollisionWorld::ContactResultCallback {
	public:
		ContactSensorCallback(btCollisionObject * object)
		: object_(object)
		, any_collision_(false)
		{
		}
		//! If you don't want to consider collisions where the bodies are joined by a constraint, override needsCollision:
		/*! However, if you use a btCollisionObject for #body instead of a btRigidBody,
		 *  then this is unnecessary—checkCollideWithOverride isn't available */
		bool needsCollision(btBroadphaseProxy* proxy) const override
		{
			// We don't want static objects to be filtered out
			return true;
		}
		//! Called with each contact for your own processing (e.g. test if contacts fall in within sensor parameters)
		btScalar addSingleResult(btManifoldPoint& cp,
			const btCollisionObjectWrapper* colObj0, int partId0, int index0,
			const btCollisionObjectWrapper* colObj1, int partId1, int index1) override
		{
			if (colObj0->m_collisionObject == object_ ||
				colObj1->m_collisionObject == object_)
				any_collision_ = true;
			return 0; // There was a planned purpose for the return value of addSingleResult, but it is not used so you can ignore it.
		}
		const bool any_collision() const
		{
			return any_collision_;
		}
	private:
		btCollisionObject * object_;
		bool any_collision_;
	};
	class ContactPairCallback : public btCollisionWorld::ContactResultCallback {
	public:
		ContactPairCallback(btCollisionObject * object1, btCollisionObject * object2)
		: object1_(object1)
		, object2_(object2)
		, any_collision_(false)
		{
		}
		//! If you don't want to consider collisions where the bodies are joined by a constraint, override needsCollision:
		/*! However, if you use a btCollisionObject for #body instead of a btRigidBody,
		 *  then this is unnecessary—checkCollideWithOverride isn't available */
		bool needsCollision(btBroadphaseProxy* proxy) const override
		{
			// We don't want static objects to be filtered out
			return true;
		}
		//! Called with each contact for your own processing (e.g. test if contacts fall in within sensor parameters)
		btScalar addSingleResult(btManifoldPoint& cp,
			const btCollisionObjectWrapper* colObj0, int partId0, int index0,
			const btCollisionObjectWrapper* colObj1, int partId1, int index1) override
		{
			if ((colObj0->m_collisionObject == object1_ && colObj1->m_collisionObject == object2_) ||
				(colObj0->m_collisionObject == object2_ && colObj1->m_collisionObject == object1_))
				any_collision_ = true;
			return 0; // There was a planned purpose for the return value of addSingleResult, but it is not used so you can ignore it.
		}
		const bool any_collision() const
		{
			return any_collision_;
		}
	private:
		btCollisionObject * object1_;
		btCollisionObject * object2_;
		bool any_collision_;
	};
}

namespace sht {
	namespace physics {

		Engine::Engine(int max_sub_steps, float fixed_time_step)
		: max_sub_steps_(max_sub_steps)
		, fixed_time_step_(fixed_time_step)
		{
			memset(&unit_conversion_, 0, sizeof(unit_conversion_));

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
		void Engine::SetLinearToFunction(ConversionFunction function)
		{
			unit_conversion_.linear_to = function;
		}
		void Engine::SetLinearFromFunction(ConversionFunction function)
		{
			unit_conversion_.linear_from = function;
		}
		void Engine::SetMassToFunction(ConversionFunction function)
		{
			unit_conversion_.mass_to = function;
		}
		void Engine::SetMassFromFunction(ConversionFunction function)
		{
			unit_conversion_.mass_from = function;
		}
		Object * Engine::AddSphere(const math::Vector3& position,
				float mass, float radius)
		{
			Sphere * object = new Sphere(position, mass, radius);
			object->CreateShape(&unit_conversion_);
			AddCustomObject(object, mass);
			return object;
		}
		Object * Engine::AddBox(const math::Vector3& position,
				float mass, float size_x, float size_y, float size_z)
		{
			Box * object = new Box(position, mass, size_x, size_y, size_z);
			object->CreateShape(&unit_conversion_);
			AddCustomObject(object, mass);
			return object;
		}
		Object * Engine::AddMesh(const math::Vector3& position,
				float mass, graphics::MeshVerticesEnumerator * enumerator)
		{
			Mesh * object = new Mesh(position, mass);
			object->CreateShape(&unit_conversion_, enumerator);
			AddCustomObject(object, mass);
			return object;
		}
		GhostObject * Engine::AddGhostObject(const math::Vector3& position,
			graphics::MeshVerticesEnumerator * enumerator)
		{
			GhostObject * ghost_object = new GhostObject(position);
			ghost_object->CreateShape(&unit_conversion_, enumerator);
			ghost_objects_.push_back(ghost_object);
			return ghost_object;
		}
		void Engine::AttachGhostObject(GhostObject * ghost_object)
		{
			dynamics_world_->addCollisionObject(ghost_object->object_);
		}
		void Engine::DetachGhostObject(GhostObject * ghost_object)
		{
			dynamics_world_->removeCollisionObject(ghost_object->object_);
		}
		bool Engine::ContactTest(GhostObject * ghost_object)
		{
			ContactSensorCallback callback(ghost_object->object_);
			dynamics_world_->contactTest(ghost_object->object_, callback);
			return callback.any_collision();
		}
		bool Engine::ContactPairTest(GhostObject * ghost_object, Object * object)
		{
			ContactPairCallback callback(ghost_object->object_, object->body_);
			dynamics_world_->contactPairTest(ghost_object->object_, object->body_, callback);
			return callback.any_collision();
		}
		void Engine::ReleaseObjects()
		{
			for (auto object : objects_)
			{
				delete object;
			}
			for (auto ghost_object : ghost_objects_)
			{
				delete ghost_object;
			}
		}
		void Engine::AddCustomObject(Object * object, float mass)
		{
			if (unit_conversion_.mass_to)
				unit_conversion_.mass_to(&mass);
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
			dynamics_world_->stepSimulation(sec, max_sub_steps_, fixed_time_step_);
		}

	} // namespace physics
} // namespace sht