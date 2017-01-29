#include "physics_logics.h"
#include "physics_motion_state.h"

namespace sht {
	namespace physics {

		Logics::Logics()
		{
			collision_configuration_ = new btDefaultCollisionConfiguration();
			dispatcher_ = new btCollisionDispatcher(collision_configuration_);
			broadphase_ = new btDbvtBroadphase();
			solver_ = new btSequentialImpulseConstraintSolver();
			dynamics_world_ = new btDiscreteDynamicsWorld(dispatcher_, broadphase_, solver_, collision_configuration_);
		}
		Logics::~Logics()
		{
			ReleaseObjects();
			delete dynamics_world_;
			delete solver_;
			delete broadphase_;
			delete dispatcher_;
			delete collision_configuration_;
		}
		void Logics::SetupObjects()
		{
			dynamics_world_->setGravity(btVector3(0.0f, -5.0f, 0.0f));
			{
				btCollisionShape* ground_shape = new btBoxShape(btVector3(50.0f, 50.0f, 50.0f));

				collision_shapes_.push_back(ground_shape);

				btTransform groundTransform;
				groundTransform.setIdentity();
				groundTransform.setOrigin(btVector3(0.0f, -50.0f, 0.0f));

				btVector3 localInertia(0.0f, 0.0f, 0.0f);

				btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
				btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, myMotionState, ground_shape, localInertia);
				btRigidBody* body = new btRigidBody(rbInfo);

				//add the body to the dynamics world
				dynamics_world_->addRigidBody(body);
			}

			{
				btCollisionShape* colShape = new btSphereShape(btScalar(1.));
				collision_shapes_.push_back(colShape);

				/// Create Dynamic Objects
				btTransform startTransform;
				startTransform.setIdentity();

				btScalar	mass(1.f);

				btVector3 localInertia(0,0,0);
				colShape->calculateLocalInertia(mass,localInertia);

				startTransform.setOrigin(btVector3(0.0f, 10.0f, 0.0f));
			
				//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
				object_motion_state_ = new MotionState(startTransform);
				btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,object_motion_state_,colShape,localInertia);
				btRigidBody* body = new btRigidBody(rbInfo);

				dynamics_world_->addRigidBody(body);
			}
		}
		void Logics::ReleaseObjects()
		{
			// Remove the rigidbodies from the dynamics world and delete them
			for (int i = dynamics_world_->getNumCollisionObjects()-1; i >= 0; --i)
			{
				btCollisionObject* obj = dynamics_world_->getCollisionObjectArray()[i];
				btRigidBody* body = btRigidBody::upcast(obj);
				if (body && body->getMotionState())
				{
					delete body->getMotionState();
				}
				dynamics_world_->removeCollisionObject(obj);
				delete obj;
			}

			// Delete collision shapes
			for (int j = 0; j < collision_shapes_.size(); ++j)
			{
				btCollisionShape* shape = collision_shapes_[j];
				collision_shapes_[j] = nullptr;
				delete shape;
			}
			collision_shapes_.clear();
		}
		void Logics::Update(float sec)
		{
			const int kMaxSubSteps = 10;
			const float kFixedTimeStep = 0.01;
			dynamics_world_->stepSimulation(sec, kMaxSubSteps, kFixedTimeStep);
		}
		void Logics::GetPosition(math::Vector3 * position)
		{
			object_motion_state_->GetPosition(position);
		}

	} // namespace physics
} // namespace sht