#pragma once
#ifndef __SHT_MATH_VECTOR_H__
#define __SHT_MATH_VECTOR_H__

namespace sht {
	namespace math {

		//! 2D single-precision floating-point vector
		struct Vector2 {

			Vector2();
			Vector2(const Vector2& v);
			Vector2(const float iv);
			Vector2(const float ix, const float iy);
            
            void Set(const float ix, const float iy);

			operator float *();
			operator const float *() const;

			Vector2& operator = (const Vector2 &v);
			void operator += (const Vector2 &v);
			void operator -= (const Vector2 &v);
			void operator *= (const float s);
			void operator *= (const Vector2 &v);
			void operator /= (const float s);
			void operator /= (const Vector2 &v);

			friend Vector2 operator + (const Vector2 &u, const Vector2 &v);
			friend Vector2 operator + (const Vector2 &v, const float s);
			friend Vector2 operator - (const Vector2 &u, const Vector2 &v);
			friend Vector2 operator - (const Vector2 &v, const float s);
			friend Vector2 operator - (const Vector2 &v);
			friend Vector2 operator * (const Vector2 &u, const Vector2 &v);
			friend Vector2 operator * (const float s, const Vector2 &v);
			friend Vector2 operator * (const Vector2 &v, const float s);
			friend Vector2 operator / (const Vector2 &u, const Vector2 &v);
			friend Vector2 operator / (const Vector2 &v, const float s);
			friend bool operator == (const Vector2 &u, const Vector2 &v);
			friend bool operator != (const Vector2 &u, const Vector2 &v);
			friend bool operator < (const Vector2 &u, const Vector2 &v);
			friend bool operator > (const Vector2 &u, const Vector2 &v);

			void Null();
			bool IsNull() const;
			float Sqr() const;
			float Length() const;
            float Distance(const Vector2& v) const;
			void Normalize();
			Vector2 GetNormalized() const;

			float x, y;
		};

		//! 3D single-precision floating-point vector
		struct Vector3 {

			Vector3();
			Vector3(const Vector3& _v);
			Vector3(const float iv);
			Vector3(const Vector2 &xy, const float iz);
			Vector3(const float ix, const float iy, const float iz);
            
            void Set(const float ix, const float iy, const float iz);

			operator float *();
			operator const float *() const;

			Vector2 xy() const;
			Vector2 xz() const;
			Vector2 yz() const;

			Vector3 xzy() const;
			Vector3 zyx() const;
			Vector3 yxz() const;
			Vector3 yzx() const;
			Vector3 zxy() const;

			Vector3 x0z() const;
			Vector3 ixyz() const;
			Vector3 xiyz() const;
			Vector3 xyiz() const;

			Vector3& operator = (const Vector3 &v);
			Vector3 operator - ();
			void operator += (const Vector3 &v);
			void operator -= (const Vector3 &v);
			void operator *= (const float s);
			void operator *= (const Vector3 &v);
			void operator /= (const float s);
			void operator /= (const Vector3 &v);

			friend Vector3 operator + (const Vector3 &u, const Vector3 &v);
			friend Vector3 operator + (const Vector3 &v, const float s);
			friend Vector3 operator - (const Vector3 &u, const Vector3 &v);
			friend Vector3 operator - (const Vector3 &v, const float s);
			friend Vector3 operator - (const Vector3 &v);
			friend Vector3 operator * (const Vector3 &u, const Vector3 &v);
			friend Vector3 operator * (const float s, const Vector3 &v);
			friend Vector3 operator * (const Vector3 &v, const float s);
			friend Vector3 operator / (const Vector3 &u, const Vector3 &v);
			friend Vector3 operator / (const Vector3 &v, const float s);

			friend bool operator == (const Vector3 &u, const Vector3 &v);
			friend bool operator != (const Vector3 &u, const Vector3 &v);

			friend float operator & (const Vector3 &u, const Vector3 &v); // dot product
			friend Vector3 operator ^ (const Vector3 &u, const Vector3 &v); // cross product

			void Null();
			bool IsNull() const;
			float Sqr() const;
			float Length() const;
            float Distance(const Vector3& v) const;
			void Normalize();
			Vector3 GetNormalized() const;

			void MakeFloor(const Vector3& other);
			void MakeCeil(const Vector3& other);

			float x, y, z;
		};

		//! 4D single-precision floating-point vector
		struct Vector4 {

			Vector4();
			Vector4(const Vector4& _v);
			Vector4(const float iv);
			Vector4(const Vector3& _v, const float _w);
			Vector4(const float _x, const float _y, const float _z, const float _w);
            
            void Set(const float _x, const float _y, const float _z, const float _w);

			operator float *();
			operator const float *() const;

			Vector3 xyz() const;

			Vector4& operator = (const Vector4 &v);
			Vector4 operator -();
			void operator += (const Vector4 &v);
			void operator -= (const Vector4 &v);
			void operator *= (const float s);
			void operator *= (const Vector4 &v);
			void operator /= (const float s);
			void operator /= (const Vector4 &v);

			friend Vector4 operator + (const Vector4 &u, const Vector4 &v);
			friend Vector4 operator + (const Vector4 &v, const float s);
			friend Vector4 operator - (const Vector4 &u, const Vector4 &v);
			friend Vector4 operator - (const Vector4 &v, const float s);
			friend Vector4 operator - (const Vector4 &v);
			friend Vector4 operator * (const Vector4 &u, const Vector4 &v);
			friend Vector4 operator * (const float s, const Vector4 &v);
			friend Vector4 operator * (const Vector4 &v, const float s);
			friend Vector4 operator / (const Vector4 &u, const Vector4 &v);
			friend Vector4 operator / (const Vector4 &v, const float s);

			friend bool operator == (const Vector4 &u, const Vector4 &v);
			friend bool operator != (const Vector4 &u, const Vector4 &v);

			friend float operator & (const Vector4 &u, const Vector4 &v); // dot product

			void Null();
			bool IsNull() const;
			float Sqr() const;
			float Length() const;
            float Distance(const Vector4& v) const;
			void Normalize();
			Vector4 GetNormalized() const;

			float x, y, z, w;
		};

	} // namespace math
} // namespace sht

typedef sht::math::Vector2 vec2;
typedef sht::math::Vector3 vec3;
typedef sht::math::Vector4 vec4;

const sht::math::Vector3 UNIT_X = sht::math::Vector3(1.0f, 0.0f, 0.0f);
const sht::math::Vector3 UNIT_Y = sht::math::Vector3(0.0f, 1.0f, 0.0f);
const sht::math::Vector3 UNIT_Z = sht::math::Vector3(0.0f, 0.0f, 1.0f);

#endif