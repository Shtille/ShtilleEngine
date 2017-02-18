#pragma once
#ifndef __SHT_PHYSICS_OBJECT_H__
#define __SHT_PHYSICS_OBJECT_H__

#include "math/vector.h"
#include "math/matrix.h"

#include "thirdparty/bullet/src/LinearMath/btMotionState.h"

class btCollisionShape;
class btRigidBody;

namespace sht {
	namespace physics {

		//! This class affects our internal representation
		class Object : private btMotionState
		{
			friend class Engine;
		public:
			explicit Object(const math::Vector3& position);
			explicit Object(const math::Vector3& position, const math::Matrix3& rotation);
			virtual ~Object();

			void SetPosition(const math::Vector3& position);
			void Translate(const math::Vector3& direction);

			void SetFriction(float friction);
			void SetRollingFriction(float friction);
			void SetSpinningFriction(float friction);
			void SetRestitution(float restitution);

			//! Make a sleeping rigid body to move under our force
			void Activate();

			void ApplyCentralForce(const math::Vector3& force);
			void ApplyForce(const math::Vector3& force, const math::Vector3& relative_position);

			void ApplyTorque(const math::Vector3& torque);

			void ApplyCentralImpulse(const math::Vector3& impulse);
			void ApplyTorqueImpulse(const math::Vector3& impulse);
			void ApplyImpulse(const math::Vector3& impulse, const math::Vector3& relative_position);

			const math::Matrix4& matrix() const;
			const math::Vector3& position() const;
			const math::Vector3 * GetPositionPtr() const;

		protected:
			btCollisionShape * shape_; //!< objects owns collision shape, so it will be destroyed on destruction
			
		private:
			//~~~ Inherited from btMotionState
			void getWorldTransform(btTransform &world_transform) const final;
			void setWorldTransform(const btTransform &world_transform) final;
			//~~~

			math::Matrix4 matrix_;
			btRigidBody * body_;
		};

	} // namespace physics
} // namespace sht

#endif