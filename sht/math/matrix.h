#pragma once
#ifndef __SHT_MATH_MATRIX_H__
#define __SHT_MATH_MATRIX_H__

#include "vector.h"

// Interface

namespace sht {
	namespace math {

		//! Single-precision floating-point matrix 3x3
		struct Matrix3 {

			Matrix3();
			Matrix3(const Matrix3& m);
			Matrix3(const Vector3& dir, const Vector3& up, const Vector3& side);
			Matrix3(const float m11, const float m12, const float m13,
				const float m21, const float m22, const float m23,
				const float m31, const float m32, const float m33);

			operator const float *() const;
			operator float *();

			Matrix3& operator = (const Matrix3 &m);
			void operator *= (const float s);
			void operator /= (const float s);
			Vector3 operator [] (const int ind);

			friend Matrix3 operator + (const Matrix3 &m1, const Matrix3 &m2);
			friend Matrix3 operator - (const Matrix3 &m1, const Matrix3 &m2);
			friend Matrix3 operator - (const Matrix3 &m);
			friend Vector3 operator * (const Matrix3 &m, const Vector3 &v);
			friend Matrix3 operator * (const Matrix3 &m1, const Matrix3 &m2);
			friend Matrix3 operator * (const float s, const Matrix3 &m);
			friend Matrix3 operator * (const Matrix3 &m, const float s);
			friend bool operator == (const Matrix3 &m1, const Matrix3 &m2);
			friend bool operator != (const Matrix3 &m1, const Matrix3 &m2);

			void Null();
			void Identity();
			void Transpose();
			float Det() const;
			float Trace() const;
			Vector3 GetDir() const;
			Vector3 GetUp() const;
			Vector3 GetSide() const;
			float* GetpDir();
			float* GetpUp();
			float* GetpSide();
			void NormalizeDir();
			void NormalizeUp();
			void NormalizeSide();
			void Normalize();
			void SetDir(const Vector3& v);
			void SetUp(const Vector3& v);
			void SetSide(const Vector3& v);
			void RotateAroundAxisX(float cos_a, float sin_a);
			void RotateAroundAxisY(float cos_a, float sin_a);
			void RotateAroundAxisZ(float cos_a, float sin_a);
			Matrix3 GetTransposed() const;
			Matrix3 GetInverse() const;

			union {
				float sa[9];			//!< 1D array
				float a[3][3];			//!< 2D array
				float e11, e12, e13,
					e21, e22, e23,
					e31, e32, e33;		//!< elements
			};
		};

		//! Single-precision floating-point matrix 4x4
		struct Matrix4 {

			Matrix4();
			Matrix4(const Matrix4& m);
			Matrix4(const float m11, const float m12, const float m13, const float m14,
				const float m21, const float m22, const float m23, const float m24,
				const float m31, const float m32, const float m33, const float m34,
				const float m41, const float m42, const float m43, const float m44);

			operator float *();
			operator const float *() const;

			Matrix4& operator = (const Matrix4 &m);
			void operator *= (const float s);
			void operator /= (const float s);
            void operator *= (const Matrix4& m);
			Vector4 operator [] (const int ind);

			friend Matrix4 operator + (const Matrix4 &m1, const Matrix4 &m2);
			friend Matrix4 operator - (const Matrix4 &m1, const Matrix4 &m2);
			friend Matrix4 operator - (const Matrix4 &m);
			friend Vector3 operator * (const Matrix4 &m, const Vector3 &v);
			friend Vector4 operator * (const Matrix4 &m, const Vector4 &v);
			friend Matrix4 operator * (const Matrix4 &m1, const Matrix4 &m2); // standart multipication
			friend Matrix4 operator ^ (const Matrix4 &m1, const Matrix4 &m2); // transposed multiplication
			friend Matrix4 operator * (const float s, const Matrix4 &m);
			friend Matrix4 operator * (const Matrix4 &m, const float s);
			friend bool operator == (const Matrix4 &m1, const Matrix4 &m2);
			friend bool operator != (const Matrix4 &m1, const Matrix4 &m2);

			void Null();
			void Identity();
			void Translate(const Vector3 &v);
			void Transpose();
			float Trace() const;
			Vector3 TransformPoint(const Vector3 &v) const;
			Vector3 TransformVector(const Vector3 &v) const;
			Matrix4 GetTransposed() const;
			Matrix4 GetInverse() const;

			union {
				float sa[16];			//!< 1D array
				float a[4][4];			//!< 2D array
				float e11, e12, e13, e14,
					e21, e22, e23, e24,
					e31, e32, e33, e34,
					e41, e42, e43, e44;		//!< elements
			};
		};

	} // namespace math
} // namespace sht

#endif