#include "matrix.h"
#include <memory.h>
#include <math.h>

namespace sht {
	namespace math {

		Matrix3::Matrix3()
		{

		}
		Matrix3::Matrix3(const Matrix3& m)
		{
			memcpy(sa, &m, sizeof(float) * 9);
		}
		Matrix3::Matrix3(const Vector3& dir, const Vector3& up, const Vector3& side)
		{
			memcpy(a[0], &dir, sizeof(float) * 3);
			memcpy(a[1], &up, sizeof(float) * 3);
			memcpy(a[2], &side, sizeof(float) * 3);
		}
		Matrix3::Matrix3(const float m11, const float m12, const float m13,
			const float m21, const float m22, const float m23,
			const float m31, const float m32, const float m33)
		{
			e11 = m11; e12 = m12; e13 = m13;
			e21 = m21; e22 = m22; e23 = m23;
			e31 = m31; e32 = m32; e33 = m33;
		}
		Matrix3::operator const float *() const
		{
			return const_cast<float*>(sa);
		}
		Matrix3::operator float *()
		{
			return sa;
		}
		Matrix3& Matrix3::operator = (const Matrix3 &m)
		{
			memcpy(sa, &m, sizeof(float) * 9);
			return *this;
		}
		void Matrix3::operator *= (const float s)
		{
			for (int i = 0; i<9; ++i)
				sa[i] *= s;
		}
		void Matrix3::operator /= (const float s)
		{
			float invs = 1.0f / s;
			for (int i = 0; i<9; ++i)
				sa[i] *= invs;
		}
		Vector3 Matrix3::operator [] (const int ind)
		{
			return Vector3(a[ind][0], a[ind][1], a[ind][2]);
		}
		Matrix3 operator + (const Matrix3 &m1, const Matrix3 &m2)
		{
			Matrix3 m;
			for (int i = 0; i < 9; ++i)
				m.sa[i] = m1.sa[i] + m2.sa[i];
			return m;
		}
		Matrix3 operator - (const Matrix3 &m1, const Matrix3 &m2)
		{
			Matrix3 m;
			for (int i = 0; i < 9; ++i)
				m.sa[i] = m1.sa[i] - m2.sa[i];
			return m;
		}
		Matrix3 operator - (const Matrix3 &m)
		{
			Matrix3 mr;
			for (int i = 0; i < 9; ++i)
				mr.sa[i] = -m.sa[i];
			return mr;
		}
		Vector3 operator * (const Matrix3 &m, const Vector3 &v)
		{
			Vector3 res;
			res.x = m.a[0][0] * v.x + m.a[1][0] * v.y + m.a[2][0] * v.z;
			res.y = m.a[0][1] * v.x + m.a[1][1] * v.y + m.a[2][1] * v.z;
			res.z = m.a[0][2] * v.x + m.a[1][2] * v.y + m.a[2][2] * v.z;
			return res;
		}
		Matrix3 operator * (const Matrix3 &m1, const Matrix3 &m2)
		{
			// we should i-row multiply by j-column
			Matrix3 mr;
			for (int i = 0; i < 3; ++i)
				for (int j = 0; j < 3; ++j)
					mr.a[j][i] = m1.a[0][i] * m2.a[j][0] + m1.a[1][i] * m2.a[j][1] + m1.a[2][i] * m2.a[j][2];
			return mr;
		}
		Matrix3 operator * (const float s, const Matrix3 &m)
		{
			Matrix3 mr;
			for (int i = 0; i < 9; ++i)
				mr.sa[i] = s * m.sa[i];
			return mr;
		}
		Matrix3 operator * (const Matrix3 &m, const float s)
		{
			Matrix3 mr;
			for (int i = 0; i < 9; ++i)
				mr.sa[i] = m.sa[i] * s;
			return mr;
		}
		bool operator == (const Matrix3 &m1, const Matrix3 &m2)
		{
			for (int i = 0; i < 9; ++i)
				if (m1.sa[i] != m2.sa[i])
					return false;
			return true;
		}
		bool operator != (const Matrix3 &m1, const Matrix3 &m2)
		{
			return !operator==(m1, m2);
		}
		void Matrix3::Null()
		{
			memset(sa, 0, sizeof(float) * 9);
		}
		void Matrix3::Identity()
		{
			a[0][0] = 1.0f; a[0][1] = 0.0f; a[0][2] = 0.0f;
			a[1][0] = 0.0f; a[1][1] = 1.0f; a[1][2] = 0.0f;
			a[2][0] = 0.0f; a[2][1] = 0.0f; a[2][2] = 1.0f;
		}
		void Matrix3::Transpose()
		{
			float t;
			for (int i = 0; i < 3; ++i)
				for (int j = 0; j < 3; ++j)
					if (i < j)
					{
						t = a[i][j];
						a[i][j] = a[j][i];
						a[j][i] = t;
					}
		}
		float Matrix3::Det() const
		{
			return (a[0][0] * a[1][1] * a[2][2]) + (a[0][1] * a[1][2] * a[2][0]) +
				(a[0][2] * a[1][0] * a[2][1]) - (a[0][2] * a[1][1] * a[2][0]) -
				(a[0][1] * a[1][0] * a[2][2]) - (a[0][0] * a[1][2] * a[2][1]);
		}
		float Matrix3::Trace() const
		{
			return e11 + e22 + e33;
		}
		Vector3 Matrix3::GetDir() const
		{
			return Vector3(e11, e12, e13);
		}
		Vector3 Matrix3::GetUp() const
		{
			return Vector3(e21, e22, e23);
		}
		Vector3 Matrix3::GetSide() const
		{
			return Vector3(e31, e32, e33);
		}
		float* Matrix3::GetpDir()
		{
			return a[0];
		}
		float* Matrix3::GetpUp()
		{
			return a[1];
		}
		float* Matrix3::GetpSide()
		{
			return a[2];
		}
		void Matrix3::NormalizeDir()
		{
			float r = 1.0f / sqrtf(e11 * e11 + e12 * e12 + e13 * e13);
			e11 *= r;
			e12 *= r;
			e13 *= r;
		}
		void Matrix3::NormalizeUp()
		{
			float r = 1.0f / sqrtf(e21 * e21 + e22 * e22 + e23 * e23);
			e21 *= r;
			e22 *= r;
			e23 *= r;
		}
		void Matrix3::NormalizeSide()
		{
			float r = 1.0f / sqrtf(e31 * e31 + e32 * e32 + e33 * e33);
			e31 *= r;
			e32 *= r;
			e33 *= r;
		}
		void Matrix3::Normalize()
		{
			NormalizeDir();
			NormalizeUp();
			NormalizeSide();
		}
		void Matrix3::SetDir(const Vector3& v)
		{
			e11 = v.x;
			e12 = v.y;
			e13 = v.z;
		}
		void Matrix3::SetUp(const Vector3& v)
		{
			e21 = v.x;
			e22 = v.y;
			e23 = v.z;
		}
		void Matrix3::SetSide(const Vector3& v)
		{
			e31 = v.x;
			e32 = v.y;
			e33 = v.z;
		}
		void Matrix3::RotateAroundAxisX(float cos_a, float sin_a)
		{
			Vector3 up = GetUp();
			Vector3 side = GetSide();
			SetSide(side*cos_a - up*sin_a); NormalizeSide();
			SetUp(side*sin_a + up*cos_a); NormalizeUp();
		}
		void Matrix3::RotateAroundAxisY(float cos_a, float sin_a)
		{
			Vector3 dir = GetDir();
			Vector3 side = GetSide();
			SetDir(dir*cos_a - side*sin_a); NormalizeDir();
			SetSide(dir*sin_a + side*cos_a); NormalizeSide();
		}
		void Matrix3::RotateAroundAxisZ(float cos_a, float sin_a)
		{
			Vector3 dir = GetDir();
			Vector3 up = GetUp();
			SetUp(up*cos_a - dir*sin_a); NormalizeUp();
			SetDir(up*sin_a + dir*cos_a); NormalizeDir();
		}
		Matrix3 Matrix3::GetTransposed() const
		{
			return Matrix3(e11, e21, e31, e12, e22, e32, e13, e23, e33);
		}
		Matrix3 Matrix3::GetInverse() const
		{
			float p0 = a[1][1] * a[2][2] - a[2][1] * a[1][2];
			float p1 = a[1][2] * a[2][0] - a[1][0] * a[2][2];
			float p2 = a[1][0] * a[2][1] - a[1][1] * a[2][0];

			float inv_det = 1.0f / (a[0][0] * p0 + a[0][1] * p1 + a[0][2] * p2);

			Matrix3 m;
			m.a[0][0] = p0;
			m.a[0][1] = a[2][1] * a[0][2] - a[0][1] * a[2][2];
			m.a[0][2] = a[0][1] * a[1][2] - a[1][1] * a[0][2];
			m.a[1][0] = p1;
			m.a[1][1] = a[0][0] * a[2][2] - a[2][0] * a[0][2];
			m.a[1][2] = a[1][0] * a[0][2] - a[0][0] * a[1][2];
			m.a[2][0] = p2;
			m.a[2][1] = a[2][0] * a[0][1] - a[0][0] * a[2][1];
			m.a[2][2] = a[0][0] * a[1][1] - a[0][1] * a[1][0];
			return m * inv_det;
		}

		Matrix4::Matrix4()
		{

		}
		Matrix4::Matrix4(const Matrix4& m)
		{
			memcpy(sa, m.sa, sizeof(float) * 16);
		}
		Matrix4::Matrix4(const float m11, const float m12, const float m13, const float m14,
			const float m21, const float m22, const float m23, const float m24,
			const float m31, const float m32, const float m33, const float m34,
			const float m41, const float m42, const float m43, const float m44)
		{
			e11 = m11; e12 = m12; e13 = m13; e14 = m14;
			e21 = m21; e22 = m22; e23 = m23; e24 = m24;
			e31 = m31; e32 = m32; e33 = m33; e34 = m34;
			e41 = m41; e42 = m42; e43 = m43; e44 = m44;
		}
		Matrix4::operator float *()
		{
			return sa;
		}
		Matrix4::operator const float *() const
		{
			return sa;
		}
		Matrix4& Matrix4::operator = (const Matrix4 &m)
		{
			memcpy(sa, m.sa, sizeof(float) * 16);
			return *this;
		}
		void Matrix4::operator *= (const float s)
		{
			for (int i = 0; i < 16; ++i)
				sa[i] *= s;
		}
		void Matrix4::operator /= (const float s)
		{
			float r = 1.0f / s;
			for (int i = 0; i < 16; ++i)
				sa[i] *= r;
		}
		Vector4 Matrix4::operator [] (const int ind)
		{
			return Vector4(a[ind][0], a[ind][1], a[ind][2], a[ind][3]);
		}
		Matrix4 operator + (const Matrix4 &m1, const Matrix4 &m2)
		{
			Matrix4 mr;
			for (int i = 0; i < 16; ++i)
				mr.sa[i] = m1.sa[i] + m2.sa[i];
			return mr;
		}
		Matrix4 operator - (const Matrix4 &m1, const Matrix4 &m2)
		{
			Matrix4 mr;
			for (int i = 0; i < 16; ++i)
				mr.sa[i] = m1.sa[i] - m2.sa[i];
			return mr;
		}
		Matrix4 operator - (const Matrix4 &m)
		{
			Matrix4 mr;
			for (int i = 0; i < 16; ++i)
				mr.sa[i] = -m.sa[i];
			return mr;
		}
		Vector3 operator * (const Matrix4 &m, const Vector3 &v)
		{
			Vector4 res;
			res.x = m.a[0][0] * v.x + m.a[1][0] * v.y + m.a[2][0] * v.z + m.a[3][0];
			res.y = m.a[0][1] * v.x + m.a[1][1] * v.y + m.a[2][1] * v.z + m.a[3][1];
			res.z = m.a[0][2] * v.x + m.a[1][2] * v.y + m.a[2][2] * v.z + m.a[3][2];
			res.w = m.a[0][3] * v.x + m.a[1][3] * v.y + m.a[2][3] * v.z + m.a[3][3];
			return Vector3(res.x / res.w, res.y / res.w, res.z / res.w);
		}
		Vector4 operator * (const Matrix4 &m, const Vector4 &v)
		{
			Vector4 res;
			res.x = m.a[0][0] * v.x + m.a[1][0] * v.y + m.a[2][0] * v.z + m.a[3][0] * v.w;
			res.y = m.a[0][1] * v.x + m.a[1][1] * v.y + m.a[2][1] * v.z + m.a[3][1] * v.w;
			res.z = m.a[0][2] * v.x + m.a[1][2] * v.y + m.a[2][2] * v.z + m.a[3][2] * v.w;
			res.w = m.a[0][3] * v.x + m.a[1][3] * v.y + m.a[2][3] * v.z + m.a[3][3] * v.w;
			return res;
		}
		Matrix4 operator * (const Matrix4 &m1, const Matrix4 &m2)
		{
			// we should i-row multiply by j-column
			Matrix4 mr;
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
				{
					mr.a[j][i] = m1.a[0][i] * m2.a[j][0];
					mr.a[j][i] += m1.a[1][i] * m2.a[j][1];
					mr.a[j][i] += m1.a[2][i] * m2.a[j][2];
					mr.a[j][i] += m1.a[3][i] * m2.a[j][3];
				}
			return mr;
		}
		Matrix4 operator ^ (const Matrix4 &m1, const Matrix4 &m2)
		{
			// we should i-column multiply by j-row
			Matrix4 mr;
			for (int j = 0; j<4; ++j)
				for (int i = 0; i<4; ++i)
				{
					mr.a[i][j] = m1.a[i][0] * m2.a[0][j];
					mr.a[i][j] += m1.a[i][1] * m2.a[1][j];
					mr.a[i][j] += m1.a[i][2] * m2.a[2][j];
					mr.a[i][j] += m1.a[i][3] * m2.a[3][j];
				}
			return mr;
		}
		Matrix4 operator * (const float s, const Matrix4 &m)
		{
			Matrix4 mr;
			for (int i = 0; i < 16; ++i)
				mr.sa[i] = s * m.sa[i];
			return mr;
		}
		Matrix4 operator * (const Matrix4 &m, const float s)
		{
			Matrix4 mr;
			for (int i = 0; i < 16; ++i)
				mr.sa[i] = m.sa[i] * s;
			return mr;
		}
		bool operator == (const Matrix4 &m1, const Matrix4 &m2)
		{
			for (int i = 0; i < 16; ++i)
				if (m1.sa[i] != m2.sa[i])
					return false;
			return true;
		}
		bool operator != (const Matrix4 &m1, const Matrix4 &m2)
		{
			return !operator==(m1, m2);
		}
		void Matrix4::Null()
		{
			memset(sa, 0, sizeof(float) * 16);
		}
		void Matrix4::Identity()
		{
			a[0][0] = 1.0f; a[0][1] = 0.0f; a[0][2] = 0.0f; a[0][3] = 0.0f;
			a[1][0] = 0.0f; a[1][1] = 1.0f; a[1][2] = 0.0f; a[1][3] = 0.0f;
			a[2][0] = 0.0f; a[2][1] = 0.0f; a[2][2] = 1.0f; a[2][3] = 0.0f;
			a[3][0] = 0.0f; a[3][1] = 0.0f; a[3][2] = 0.0f; a[3][3] = 1.0f;
		}
		void Matrix4::Translate(const Vector3 &v)
		{
			a[3][0] += (a[0][0] * v.x + a[1][0] * v.y + a[2][0] * v.z);
			a[3][1] += (a[0][1] * v.x + a[1][1] * v.y + a[2][1] * v.z);
			a[3][2] += (a[0][2] * v.x + a[1][2] * v.y + a[2][2] * v.z);
			a[3][3] += (a[0][3] * v.x + a[1][3] * v.y + a[2][3] * v.z);
		}
		void Matrix4::Transpose()
		{
			float t;
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					if (i < j)
					{
						t = a[i][j];
						a[i][j] = a[j][i];
						a[j][i] = t;
					}
		}
		float Matrix4::Trace() const
		{
			return a[0][0] + a[1][1] + a[2][2] + a[3][3];
		}
		Vector3 Matrix4::TransformPoint(const Vector3 &v) const
		{
			Vector3 rv;
			rv.x = a[0][0] * v.x + a[1][0] * v.y + a[2][0] * v.z + a[3][0];
			rv.y = a[0][1] * v.x + a[1][1] * v.y + a[2][1] * v.z + a[3][1];
			rv.z = a[0][2] * v.x + a[1][2] * v.y + a[2][2] * v.z + a[3][2];
			return rv;
		}
		Vector3 Matrix4::TransformVector(const Vector3 &v) const
		{
			Vector3 rv;
			rv.x = a[0][0] * v.x + a[1][0] * v.y + a[2][0] * v.z;
			rv.y = a[0][1] * v.x + a[1][1] * v.y + a[2][1] * v.z;
			rv.z = a[0][2] * v.x + a[1][2] * v.y + a[2][2] * v.z;
			return rv;
		}
		Matrix4 Matrix4::GetTransposed() const
		{
			return Matrix4(a[0][0], a[1][0], a[2][0], a[3][0],
				a[0][1], a[1][1], a[2][1], a[3][1],
				a[0][2], a[1][2], a[2][2], a[3][2],
				a[0][3], a[1][3], a[2][3], a[3][3]);
		}
		Matrix4 Matrix4::GetInverse() const
		{
			Matrix4 mat;
			float p00 = a[2][2] * a[3][3];
			float p01 = a[3][2] * a[2][3];
			float p02 = a[2][1] * a[3][3];
			float p03 = a[2][3] * a[3][1];
			float p04 = a[2][1] * a[3][2];
			float p05 = a[2][2] * a[3][1];
			float p06 = a[2][0] * a[3][3];
			float p07 = a[2][3] * a[3][0];
			float p08 = a[2][0] * a[3][2];
			float p09 = a[2][2] * a[3][0];
			float p10 = a[2][0] * a[3][1];
			float p11 = a[2][1] * a[3][0];

			mat.a[0][0] = (p00 * a[1][1] + p03 * a[1][2] + p04 * a[1][3]) - (p01 * a[1][1] + p02 * a[1][2] + p05 * a[1][3]);
			mat.a[1][0] = (p01 * a[1][0] + p06 * a[1][2] + p09 * a[1][3]) - (p00 * a[1][0] + p07 * a[1][2] + p08 * a[1][3]);
			mat.a[2][0] = (p02 * a[1][0] + p07 * a[1][1] + p10 * a[1][3]) - (p03 * a[1][0] + p06 * a[1][1] + p11 * a[1][3]);
			mat.a[3][0] = (p05 * a[1][0] + p08 * a[1][1] + p11 * a[1][2]) - (p04 * a[1][0] + p09 * a[1][1] + p10 * a[1][2]);
			mat.a[0][1] = (p01 * a[0][1] + p02 * a[0][2] + p05 * a[0][3]) - (p00 * a[0][1] + p03 * a[0][2] + p04 * a[0][3]);
			mat.a[1][1] = (p00 * a[0][0] + p07 * a[0][2] + p08 * a[0][3]) - (p01 * a[0][0] + p06 * a[0][2] + p09 * a[0][3]);
			mat.a[2][1] = (p03 * a[0][0] + p06 * a[0][1] + p11 * a[0][3]) - (p02 * a[0][0] + p07 * a[0][1] + p10 * a[0][3]);
			mat.a[3][1] = (p04 * a[0][0] + p09 * a[0][1] + p10 * a[0][2]) - (p05 * a[0][0] + p08 * a[0][1] + p11 * a[0][2]);

			float q00 = a[0][2] * a[1][3];
			float q01 = a[0][3] * a[1][2];
			float q02 = a[0][1] * a[1][3];
			float q03 = a[0][3] * a[1][1];
			float q04 = a[0][1] * a[1][2];
			float q05 = a[0][2] * a[1][1];
			float q06 = a[0][0] * a[1][3];
			float q07 = a[0][3] * a[1][0];
			float q08 = a[0][0] * a[1][2];
			float q09 = a[0][2] * a[1][0];
			float q10 = a[0][0] * a[1][1];
			float q11 = a[0][1] * a[1][0];

			mat.a[0][2] = (q00 * a[3][1] + q03 * a[3][2] + q04 * a[3][3]) - (q01 * a[3][1] + q02 * a[3][2] + q05 * a[3][3]);
			mat.a[1][2] = (q01 * a[3][0] + q06 * a[3][2] + q09 * a[3][3]) - (q00 * a[3][0] + q07 * a[3][2] + q08 * a[3][3]);
			mat.a[2][2] = (q02 * a[3][0] + q07 * a[3][1] + q10 * a[3][3]) - (q03 * a[3][0] + q06 * a[3][1] + q11 * a[3][3]);
			mat.a[3][2] = (q05 * a[3][0] + q08 * a[3][1] + q11 * a[3][2]) - (q04 * a[3][0] + q09 * a[3][1] + q10 * a[3][2]);
			mat.a[0][3] = (q02 * a[2][2] + q05 * a[2][3] + q01 * a[2][1]) - (q04 * a[2][3] + q00 * a[2][1] + q03 * a[2][2]);
			mat.a[1][3] = (q08 * a[2][3] + q00 * a[2][0] + q07 * a[2][2]) - (q06 * a[2][2] + q09 * a[2][3] + q01 * a[2][0]);
			mat.a[2][3] = (q06 * a[2][1] + q11 * a[2][3] + q03 * a[2][0]) - (q10 * a[2][3] + q02 * a[2][0] + q07 * a[2][1]);
			mat.a[3][3] = (q10 * a[2][2] + q04 * a[2][0] + q09 * a[2][1]) - (q08 * a[2][1] + q11 * a[2][2] + q05 * a[2][0]);

			return mat * (1.0f / (a[0][0] * mat.a[0][0] + a[0][1] * mat.a[1][0] + a[0][2] * mat.a[2][0] + a[0][3] * mat.a[3][0]));
		}

	} // namespace math
} // namespace sht