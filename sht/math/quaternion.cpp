#include "quaternion.h"
#include <math.h>

namespace sht {
	namespace math {

		Quaternion::Quaternion()
		{

		}
		Quaternion::Quaternion(const float ix, const float iy, const float iz, const float iw)
		{
			x = ix;
			y = iy;
			z = iz;
			w = iw;
		}
		Quaternion::Quaternion(const Vector3& vector, const float angle)
		{
			Set(vector, angle);
		}
		Quaternion::Quaternion(const Vector3& from, const Vector3& to)
		{
			Set(from, to);
		}
		Quaternion& Quaternion::operator = (const Quaternion& q)
		{
			x = q.x;
			y = q.y;
			z = q.z;
			w = q.w;
			return *this;
		}
		void Quaternion::operator *= (const float s)
		{
			x *= s;
			y *= s;
			z *= s;
			w *= s;
		}
		void Quaternion::operator /= (const float s)
		{
			float r = 1.0f / s;
			x *= r;
			y *= r;
			z *= r;
			w *= r;
		}
		Quaternion operator + (const Quaternion &q1, const Quaternion &q2)
		{
			return Quaternion(q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w);
		}
		Quaternion operator - (const Quaternion &q1, const Quaternion &q2)
		{
			return Quaternion(q1.x - q2.x, q1.y - q2.y, q1.z - q2.z, q1.w - q2.w);
		}
		Quaternion operator - (const Quaternion &q)
		{
			return Quaternion(-q.x, -q.y, -q.z, -q.w);
		}
		Quaternion operator * (const Quaternion &q1, const Quaternion &q2)
		{
			Vector3 v1 = Vector3(q1.x, q1.y, q1.z);
			Vector3 v2 = Vector3(q2.x, q2.y, q2.z);
			return Quaternion((v1^v2) + q1.w*v2 + q2.w*v1, q1.w*q2.w - (v1&v2));
		}
		Quaternion operator * (const float s, const Quaternion &q)
		{
			return Quaternion(s * q.x, s * q.y, s * q.z, s * q.w);
		}
		Quaternion operator * (const Quaternion &q, const float s)
		{
			return Quaternion(q.x * s, q.y * s, q.z * s, q.w * s);
		}
		Quaternion operator / (const Quaternion &q, const float s)
		{
			float r = 1.0f / s;
			return Quaternion(q.x * r, q.y * r, q.z * r, q.w * r);
		}
		float operator & (const Quaternion &q1, const Quaternion &q2)
		{
			return q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;
		}
		bool operator == (const Quaternion &q1, const Quaternion &q2)
		{
			return q1.x == q2.x && q1.y == q2.y && q1.z == q2.z && q1.w == q2.w;
		}
		bool operator != (const Quaternion &q1, const Quaternion &q2)
		{
			return q1.x != q2.x || q1.y != q2.y || q1.z != q2.z || q1.w != q2.w;
		}
		Vector3 Quaternion::xyz() const
		{
			return Vector3(x, y, z);
		}
		Vector3 Quaternion::Vector() const
		{
			return Vector3(x, y, z) / sqrt(1.0f - w*w);
		}
		void Quaternion::Null()
		{
			x = y = z = w = 0.0f;
		}
		float Quaternion::Length() const
		{
			return sqrt(x*x + y*y + z*z + w*w);
		}
		float Quaternion::Norm() const
		{
			return x*x + y*y + z*z + w*w;
		}
		float Quaternion::Angle() const
		{
			if (w < 0.0f)
				return 2.0f*atan2(-sqrt(x*x + y*y + z*z), -w);
			else
				return 2.0f*atan2(sqrt(x*x + y*y + z*z), w);
		}
		float Quaternion::HalfAngle() const
		{
			if (w < 0.0f)
				return atan2(-sqrt(x*x + y*y + z*z), -w);
			else
				return atan2(sqrt(x*x + y*y + z*z), w);
		}
		void Quaternion::Identity()
		{
			x = y = z = 0.0f;
			w = 1.0f;
		}
		void Quaternion::Set(const Vector3& vector, const float angle)
		{
			float halfangle = angle * 0.5f;
			float sin_ha = sin(halfangle);
			x = vector.x * sin_ha;
			y = vector.y * sin_ha;
			z = vector.z * sin_ha;
			w = cos(halfangle);
		}
		void Quaternion::Set(const Vector3& from, const Vector3& to)
		{
			Vector3 dir = to - from;
			dir.Normalize();
			float cos_angle = dir.x; // dir dot i
			const float kPrecision = 0.001f; // TODO: move to separate module
			if (cos_angle + kPrecision >= 1.0f) // not zero angle
			{
				float angle = acos(cos_angle);
				// axis = i x dir
				Vector3 axis(0.0f, -dir.z, dir.y);
				axis.Normalize();
				Set(axis, angle);
			}
			else // angle = 0
			{
				Identity();
			}
		}
		void Quaternion::Normalize()
		{
			float r = 1.0f / Length();
			x *= r;
			y *= r;
			z *= r;
			w *= r;
		}
		Quaternion Quaternion::GetNormalized() const
		{
			return (*this) / Length();
		}
		Quaternion Quaternion::GetConjugate() const
		{
			return Quaternion(-x, -y, -z, w);
		}
		Quaternion Quaternion::GetInverse() const
		{
			return GetConjugate() / Norm();
		}
		Vector3 Quaternion::RotateVector(const Vector3& _v) const
		{
			return ((*this) * Quaternion(_v.x, _v.y, _v.z, 0.0f) * GetInverse()).xyz();
		}

	} // namespace math
} // namespace sht