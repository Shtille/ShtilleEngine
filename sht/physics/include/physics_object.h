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

		struct UnitConversion;

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
			void SetTransform(const math::Matrix4& transform);

			void MakeKinematic();

			void SetFriction(float friction);
			void SetRollingFriction(float friction);
			void SetSpinningFriction(float friction);
			void SetRestitution(float restitution);
			void SetDamping(float linear, float angular);
			void SetContactProcessingThreshold(float threshold);
			void SetContactStiffnessAndDamping(float stiffness, float damping);

			//
			float GetContactProcessingThreshold();
			float GetContactStiffness();
			float GetContactDamping();

			//! Make a sleeping rigid body to move under our force
			void Activate();
			bool IsActive();
			void DisableDeactivation();

			void SetLinearVelocity(const math::Vector3& velocity);
			void SetAngularVelocity(const math::Vector3& velocity);

			void ClampLinearVelocity(float max_speed);
			void ClampAngularVelocity(float max_speed);

			void ApplyCentralForce(const math::Vector3& force);
			void ApplyForce(const math::Vector3& force, const math::Vector3& relative_position);

			void ApplyTorque(const math::Vector3& torque);

			void ApplyCentralImpulse(const math::Vector3& impulse);
			void ApplyTorqueImpulse(const math::Vector3& impulse);
			void ApplyImpulse(const math::Vector3& impulse, const math::Vector3& relative_position);

			const math::Matrix4& matrix() const;
			const math::Vector3& position() const;
			const math::Vector3 * GetPositionPtr() const;
			const math::Vector3& scale() const;
			const math::Vector3 velocity() const;

			void set_scale(const math::Vector3& scale);

		protected:
			btCollisionShape * shape_; //!< objects owns collision shape, so it will be destroyed on destruction
			
		private:
			//~~~ Inherited from btMotionState
			void getWorldTransform(btTransform &world_transform) const final;
			void setWorldTransform(const btTransform &world_transform) final;
			//~~~
			void ApplyScale(); //!< called from engine after object creation and on matrix change

			math::Matrix4 matrix_;
			math::Vector3 scale_;
			btRigidBody * body_;
			bool use_scale_; //!< scale is necessary when model has been scaled from unit size
			bool is_kinematic_;

		protected:
			const UnitConversion * unit_conversion_;
		};

	} // namespace physics
} // namespace sht

#endif