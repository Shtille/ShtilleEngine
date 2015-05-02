#include "vector.h"
#include <math.h>

namespace sht {
	namespace math {

		Vector2::Vector2()
		{
		}
		Vector2::Vector2(const float iv)
		{
			x = y = iv;
		}
		Vector2::Vector2(const float ix, const float iy)
		{
			x = ix;
			y = iy;
		}
		Vector2::operator float *()
		{
			return &x;
		}
		Vector2::operator const float *() const
		{
			return &x;
		}
		Vector2& Vector2::operator = (const Vector2 &v)
		{
			x = v.x;
			y = v.y;
			return *this;
		}
		void Vector2::operator += (const Vector2 &v)
		{
			x += v.x;
			y += v.y;
		}
		void Vector2::operator -= (const Vector2 &v)
		{
			x -= v.x;
			y -= v.y;
		}
		void Vector2::operator *= (const float s)
		{
			x *= s;
			y *= s;
		}
		void Vector2::operator *= (const Vector2 &v)
		{
			x *= v.x;
			y *= v.y;
		}
		void Vector2::operator /= (const float s)
		{
			x /= s;
			y /= s;
		}
		void Vector2::operator /= (const Vector2 &v)
		{
			x /= v.x;
			y /= v.y;
		}
		void Vector2::Null()
		{
			x = y = 0.0f;
		}
		bool Vector2::IsNull() const
		{
			return x == 0.0f && y == 0.0f;
		}
		float Vector2::Sqr() const
		{
			return x*x + y*y;
		}
		float Vector2::Length() const
		{
			return sqrt(x*x + y*y);
		}
		void Vector2::Normalize()
		{
			float r = 1.0f / Length();
			x *= r;
			y *= r;
		}
		Vector2 Vector2::GetNormalized() const
		{
			return (*this) * (1.0f / Length());
		}
		Vector2 operator + (const Vector2 &u, const Vector2 &v)
		{
			return Vector2(u.x + v.x, u.y + v.y);
		}
		Vector2 operator + (const Vector2 &v, const float s)
		{
			return Vector2(v.x + s, v.y + s);
		}
		Vector2 operator - (const Vector2 &u, const Vector2 &v)
		{
			return Vector2(u.x - v.x, u.y - v.y);
		}
		Vector2 operator - (const Vector2 &v, const float s)
		{
			return Vector2(v.x - s, v.y - s);
		}
		Vector2 operator - (const Vector2 &v)
		{
			return Vector2(-v.x, -v.y);
		}
		Vector2 operator * (const Vector2 &u, const Vector2 &v)
		{
			return Vector2(u.x * v.x, u.y * v.y);
		}
		Vector2 operator * (const float s, const Vector2 &v)
		{
			return Vector2(v.x * s, v.y * s);
		}
		Vector2 operator * (const Vector2 &v, const float s)
		{
			return Vector2(v.x * s, v.y * s);
		}
		Vector2 operator / (const Vector2 &u, const Vector2 &v)
		{
			return Vector2(u.x / v.x, u.y / v.y);
		}
		Vector2 operator / (const Vector2 &v, const float s)
		{
			return Vector2(v.x / s, v.y / s);
		}
		bool operator == (const Vector2 &u, const Vector2 &v)
		{
			return u.x == v.x && u.y == v.y;
		}
		bool operator != (const Vector2 &u, const Vector2 &v)
		{
			return u.x != v.x || u.y != v.y;
		}
		bool operator < (const Vector2 &u, const Vector2 &v)
		{
			return u.x < v.x && u.y < v.y;
		}
		bool operator >(const Vector2 &u, const Vector2 &v)
		{
			return u.x > v.x && u.y > v.y;
		}

		Vector3::Vector3()
		{
		}
		Vector3::Vector3(const Vector3& _v)
		{
			x = _v.x;
			y = _v.y;
			z = _v.z;
		}
		Vector3::Vector3(const float iv)
		{
			x = y = z = iv;
		}
		Vector3::Vector3(const Vector2 &xy, const float iz)
		{
			x = xy.x;
			y = xy.y;
			z = iz;
		}
		Vector3::Vector3(const float ix, const float iy, const float iz)
		{
			x = ix;
			y = iy;
			z = iz;
		}
		Vector3::operator float *()
		{
			return &x;
		}
		Vector3::operator const float *() const
		{
			return &x;
		}
		Vector2 Vector3::xy() const
		{
			return Vector2(x, y);
		}
		Vector2 Vector3::xz() const
		{
			return Vector2(x, z);
		}
		Vector2 Vector3::yz() const
		{
			return Vector2(y, z);
		}
		Vector3 Vector3::xzy() const
		{
			return Vector3(x, z, y);
		}
		Vector3 Vector3::zyx() const
		{
			return Vector3(z, y, x);
		}
		Vector3 Vector3::yxz() const
		{
			return Vector3(y, x, z);
		}
		Vector3 Vector3::yzx() const
		{
			return Vector3(y, z, x);
		}
		Vector3 Vector3::zxy() const
		{
			return Vector3(z, x, y);
		}
		Vector3 Vector3::x0z() const
		{
			return Vector3(x, 0.0f, z);
		}
		Vector3 Vector3::ixyz() const
		{
			return Vector3(-x, y, z);
		}
		Vector3 Vector3::xiyz() const
		{
			return Vector3(x, -y, z);
		}
		Vector3 Vector3::xyiz() const
		{
			return Vector3(x, y, -z);
		}
		Vector3& Vector3::operator = (const Vector3 &v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			return *this;
		}
		Vector3 Vector3::operator - ()
		{
			return Vector3(-x, -y, -z);
		}
		void Vector3::operator += (const Vector3 &v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
		}
		void Vector3::operator -= (const Vector3 &v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
		}
		void Vector3::operator *= (const float s)
		{
			x *= s;
			y *= s;
			z *= s;
		}
		void Vector3::operator *= (const Vector3 &v)
		{
			x *= v.x;
			y *= v.y;
			z *= v.z;
		}
		void Vector3::operator /= (const float s)
		{
			x /= s;
			y /= s;
			z /= s;
		}
		void Vector3::operator /= (const Vector3 &v)
		{
			x /= v.x;
			y /= v.y;
			z /= v.z;
		}
		void Vector3::Null()
		{
			x = y = z = 0.0f;
		}
		bool Vector3::IsNull() const
		{
			return (x == 0.0f) && (y == 0.0f) && (z == 0.0f);
		}
		float Vector3::Sqr() const
		{
			return x*x + y*y + z*z;
		}
		float Vector3::Length() const
		{
			return sqrt(x*x + y*y + z*z);
		}
		void Vector3::Normalize()
		{
			float r = 1.0f / Length();
			x *= r;
			y *= r;
			z *= r;
		}
		Vector3 Vector3::GetNormalized() const
		{
			return (*this) * (1.0f / Length());
		}
		Vector3 operator + (const Vector3 &u, const Vector3 &v)
		{
			return Vector3(u.x + v.x, u.y + v.y, u.z + v.z);
		}
		Vector3 operator + (const Vector3 &v, const float s)
		{
			return Vector3(v.x + s, v.y + s, v.z + s);
		}
		Vector3 operator - (const Vector3 &u, const Vector3 &v)
		{
			return Vector3(u.x - v.x, u.y - v.y, u.z - v.z);
		}
		Vector3 operator - (const Vector3 &v, const float s)
		{
			return Vector3(v.x - s, v.y - s, v.z - s);
		}
		Vector3 operator - (const Vector3 &v)
		{
			return Vector3(-v.x, -v.y, -v.z);
		}
		Vector3 operator * (const Vector3 &u, const Vector3 &v)
		{
			return Vector3(u.x * v.x, u.y * v.y, u.z * v.z);
		}
		Vector3 operator * (const float s, const Vector3 &v)
		{
			return Vector3(v.x * s, v.y * s, v.z * s);
		}
		Vector3 operator * (const Vector3 &v, const float s)
		{
			return Vector3(v.x * s, v.y * s, v.z * s);
		}
		Vector3 operator / (const Vector3 &u, const Vector3 &v)
		{
			return Vector3(u.x / v.x, u.y / v.y, u.z / v.z);
		}
		Vector3 operator / (const Vector3 &v, const float s)
		{
			return Vector3(v.x / s, v.y / s, v.z / s);
		}
		bool operator == (const Vector3 &u, const Vector3 &v)
		{
			return (u.x == v.x && u.y == v.y && u.z == v.z);
		}
		bool operator != (const Vector3 &u, const Vector3 &v)
		{
			return (u.x != v.x || u.y != v.y || u.z != v.z);
		}
		float operator & (const Vector3 &u, const Vector3 &v)
		{
			return (u.x*v.x + u.y*v.y + u.z*v.z);
		}
		Vector3 operator ^ (const Vector3 &u, const Vector3 &v)
		{
			Vector3 w;
			w.x = (u.y*v.z) - (u.z*v.y);
			w.y = (u.z*v.x) - (u.x*v.z);
			w.z = (u.x*v.y) - (u.y*v.x);
			return w;
		}

		Vector4::Vector4()
		{
		}
		Vector4::Vector4(const Vector4& _v)
		{
			x = _v.x;
			y = _v.y;
			z = _v.z;
			w = _v.w;
		}
		Vector4::Vector4(const float iv)
		{
			x = y = z = w = iv;
		}
		Vector4::Vector4(const Vector3& _v, const float _w)
		{
			x = _v.x;
			y = _v.y;
			z = _v.z;
			w = _w;
		}
		Vector4::Vector4(const float _x, const float _y, const float _z, const float _w)
		{
			x = _x;
			y = _y;
			z = _z;
			w = _w;
		}
		Vector4::operator float *()
		{
			return &x;
		}
		Vector4::operator const float *() const
		{
			return &x;
		}
		Vector3 Vector4::xyz() const
		{
			return Vector3(x, y, z);
		}
		Vector4& Vector4::operator = (const Vector4 &v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			w = v.w;
			return *this;
		}
		Vector4 Vector4::operator -()
		{
			return Vector4(-x, -y, -z, -w);
		}
		void Vector4::operator += (const Vector4 &v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
			w += v.w;
		}
		void Vector4::operator -= (const Vector4 &v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
			w -= v.w;
		}
		void Vector4::operator *= (const float s)
		{
			x *= s;
			y *= s;
			z *= s;
			w *= s;
		}
		void Vector4::operator *= (const Vector4 &v)
		{
			x *= v.x;
			y *= v.y;
			z *= v.z;
			w *= v.w;
		}
		void Vector4::operator /= (const float s)
		{
			x /= s;
			y /= s;
			z /= s;
			w /= s;
		}
		void Vector4::operator /= (const Vector4 &v)
		{
			x /= v.x;
			y /= v.y;
			z /= v.z;
			w /= v.w;
		}
		void Vector4::Null()
		{
			x = y = z = w = 0.0f;
		}
		bool Vector4::IsNull() const
		{
			return (x == 0.0f) && (y == 0.0f) && (z == 0.0f) && (w == 0.0f);
		}
		float Vector4::Sqr() const
		{
			return x*x + y*y + z*z + w*w;
		}
		float Vector4::Length() const
		{
			return sqrt(x*x + y*y + z*z + w*w);
		}
		void Vector4::Normalize()
		{
			float r = 1.0f / Length();
			x *= r;
			y *= r;
			z *= r;
			w *= r;
		}
		Vector4 Vector4::GetNormalized() const
		{
			return (*this) * (1.0f / Length());
		}
		Vector4 operator + (const Vector4 &u, const Vector4 &v)
		{
			return Vector4(u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w);
		}
		Vector4 operator + (const Vector4 &v, const float s)
		{
			return Vector4(v.x + s, v.y + s, v.z + s, v.w + s);
		}
		Vector4 operator - (const Vector4 &u, const Vector4 &v)
		{
			return Vector4(u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w);
		}
		Vector4 operator - (const Vector4 &v, const float s)
		{
			return Vector4(v.x - s, v.y - s, v.z - s, v.w - s);
		}
		Vector4 operator - (const Vector4 &v)
		{
			return Vector4(-v.x, -v.y, -v.z, -v.w);
		}
		Vector4 operator * (const Vector4 &u, const Vector4 &v)
		{
			return Vector4(u.x * v.x, u.y * v.y, u.z * v.z, u.w * v.w);
		}
		Vector4 operator * (const float s, const Vector4 &v)
		{
			return Vector4(v.x * s, v.y * s, v.z * s, v.w * s);
		}
		Vector4 operator * (const Vector4 &v, const float s)
		{
			return Vector4(v.x * s, v.y * s, v.z * s, v.w * s);
		}
		Vector4 operator / (const Vector4 &u, const Vector4 &v)
		{
			return Vector4(u.x / v.x, u.y / v.y, u.z / v.z, u.w / v.w);
		}
		Vector4 operator / (const Vector4 &v, const float s)
		{
			return Vector4(v.x / s, v.y / s, v.z / s, v.w / s);
		}
		bool operator == (const Vector4 &u, const Vector4 &v)
		{
			return u.x == v.x && u.y == v.y && u.z == v.z && u.w == v.w;
		}
		bool operator != (const Vector4 &u, const Vector4 &v)
		{
			return u.x != v.x || u.y != v.y || u.z != v.z || u.w != v.w;
		}
		float operator & (const Vector4 &u, const Vector4 &v)
		{
			return u.x*v.x + u.y*v.y + u.z*v.z + u.w*v.w;
		}

	} // namespace math
} // namespace sht