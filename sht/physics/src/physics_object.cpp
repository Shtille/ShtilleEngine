#include "../include/physics_object.h"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include <assert.h>

namespace sht {
	namespace physics {

		Object::Object(const math::Vector3& position)
		: shape_(nullptr)
		, body_(nullptr)
		{
			matrix_.sa[0] = 1.0f;
			matrix_.sa[1] = 0.0f;
			matrix_.sa[2] = 0.0f;
			matrix_.sa[3] = 0.0f;
			matrix_.sa[4] = 0.0f;
			matrix_.sa[5] = 1.0f;
			matrix_.sa[6] = 0.0f;
			matrix_.sa[7] = 0.0f;
			matrix_.sa[8] = 0.0f;
			matrix_.sa[9] = 0.0f;
			matrix_.sa[10] = 1.0f;
			matrix_.sa[11] = 0.0f;
			matrix_.sa[12] = position.x;
			matrix_.sa[13] = position.y;
			matrix_.sa[14] = position.z;
			matrix_.sa[15] = 1.0f;
		}
		Object::Object(const math::Vector3& position, const math::Matrix3& rotation)
		: shape_(nullptr)
		{
			matrix_.sa[0] = rotation.e11;
			matrix_.sa[1] = rotation.e12;
			matrix_.sa[2] = rotation.e13;
			matrix_.sa[3] = 0.0f;
			matrix_.sa[4] = rotation.e21;
			matrix_.sa[5] = rotation.e22;
			matrix_.sa[6] = rotation.e23;
			matrix_.sa[7] = 0.0f;
			matrix_.sa[8] = rotation.e31;
			matrix_.sa[9] = rotation.e32;
			matrix_.sa[10] = rotation.e33;
			matrix_.sa[11] = 0.0f;
			matrix_.sa[12] = position.x;
			matrix_.sa[13] = position.y;
			matrix_.sa[14] = position.z;
			matrix_.sa[15] = 1.0f;
		}
		Object::~Object()
		{
			if (shape_)
				delete shape_;
		}
		void Object::getWorldTransform(btTransform &world_transform) const
		{
			world_transform.setFromOpenGLMatrix(matrix_.sa);
		}
		void Object::setWorldTransform(const btTransform &world_transform)
		{
			world_transform.getOpenGLMatrix(matrix_.sa);
		}
		void Object::SetPosition(const math::Vector3& position)
		{
			math::Vector3 direction(position);
			direction.x -= matrix_.sa[12];
			direction.y -= matrix_.sa[13];
			direction.z -= matrix_.sa[14];
			body_->translate(btVector3(direction.x, direction.y, direction.z));
		}
		void Object::Translate(const math::Vector3& direction)
		{
			body_->translate(btVector3(direction.x, direction.y, direction.z));
		}
		void Object::SetFriction(float friction)
		{
			body_->setFriction(friction);
		}
		void Object::SetRollingFriction(float friction)
		{
			body_->setRollingFriction(friction);
		}
		void Object::SetSpinningFriction(float friction)
		{
			body_->setSpinningFriction(friction);
		}
		void Object::SetRestitution(float restitution)
		{
			body_->setRestitution(restitution);
		}
		void Object::Activate()
		{
			assert(body_);
			body_->activate();
		}
		void Object::ApplyCentralForce(const math::Vector3& force)
		{
			body_->applyCentralForce(btVector3(force.x, force.y, force.z));
		}
		void Object::ApplyForce(const math::Vector3& force, const math::Vector3& rel_pos)
		{
			body_->applyForce(btVector3(force.x, force.y, force.z),
							  btVector3(rel_pos.x, rel_pos.y, rel_pos.z));
		}
		void Object::ApplyTorque(const math::Vector3& torque)
		{
			body_->applyTorque(btVector3(torque.x, torque.y, torque.z));
		}
		void Object::ApplyCentralImpulse(const math::Vector3& impulse)
		{
			body_->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
		}
		void Object::ApplyTorqueImpulse(const math::Vector3& impulse)
		{
			body_->applyTorqueImpulse(btVector3(impulse.x, impulse.y, impulse.z));
		}
		void Object::ApplyImpulse(const math::Vector3& impulse, const math::Vector3& rel_pos)
		{
			body_->applyImpulse(btVector3(impulse.x, impulse.y, impulse.z),
								btVector3(rel_pos.x, rel_pos.y, rel_pos.z));
		}
		const math::Matrix4& Object::matrix() const
		{
			return matrix_;
		}
		const math::Vector3& Object::position() const
		{
			// Vector3 is a simple struct, thus we can use reinterpret_cast here
			return * reinterpret_cast<const math::Vector3*>(matrix_.sa + 12);
		}
		const math::Vector3 * Object::GetPositionPtr() const
		{
			// Vector3 is a simple struct, thus we can use reinterpret_cast here
			return reinterpret_cast<const math::Vector3*>(matrix_.sa + 12);
		}

	} // namespace physics
} // namespace sht