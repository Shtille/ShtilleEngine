#include "sht_math.h"
#include <math.h>

namespace sht {
	namespace math {

		Vector3 ClosestPointOnLine(const Vector3& a, const Vector3& b, const Vector3& p)
		{
			Vector3 c = p - a;
			Vector3 v = b - a;
			float d = v.Length();
			v /= d; // normalize
			float t = v & c;

			// проверка на выход за границы отрезка
			if (t < 0.0f)
				return a;
			if (t > d)
				return b;

			// Вернем точку между a и b
			v *= t;
			return (a + v);
		}
		Vector3 Orthogonalize(const Vector3& v1, const Vector3& v2)
		{
			Vector3 v2ProjV1 = ClosestPointOnLine(v1, -v1, v2);
			Vector3 res = v2 - v2ProjV1;
			res.Normalize();
			return res;
		}
		Matrix4 OrthoMatrix(float left, float right, float bottom, float top, float zNear, float zFar)
		{
			float rl = right - left;
			float tb = top - bottom;
			float fn = zFar - zNear;

			Matrix4 mat(
				2.0f / rl, 0.0f, 0.0f, 0.0f,
				0.0f, 2.0f / tb, 0.0f, 0.0f,
				0.0f, 0.0f, -2.0f / fn, 0.0f,
				-(right + left) / rl, -(top + bottom) / tb, -(zFar + zNear) / fn, 1.0f);

			return mat;
		}
		Matrix4 PerspectiveMatrix(float fov, int width, int height, float zNear, float zFar)
		{
			const float DEG_TO_RAD = 0.0174532925f;
			float h = tanf(0.5f * fov * DEG_TO_RAD);
			float w = (h * width) / height;

			Matrix4 mat(
				1.0f / w, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f / h, 0.0f, 0.0f,
				0.0f, 0.0f, (zFar + zNear) / (zNear - zFar), -1.0f,
				0.0f, 0.0f, (2.0f * zFar * zNear) / (zNear - zFar), 0.0f);

			return mat;
		}
		Matrix4 LookAt(const Vector3& from, const Vector3& at)
		{
			Vector3 dir = (at - from).GetNormalized();
			Vector3 up, side;
			if (dir != UNIT_Y)
			{
				side = dir ^ UNIT_Y;
				side.Normalize();
				up = side ^ dir;
			}
			else
			{
				up = -UNIT_X;
				side = UNIT_Z;
			}
			Matrix4 mat(
				side.x, up.x, -dir.x, 0.0f,
				side.y, up.y, -dir.y, 0.0f,
				side.z, up.z, -dir.z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
			mat.Translate(-from);

			return mat;
		}
		Matrix4 LookAtCube(const Vector3& from, int face)
		{
			Matrix4 mat;
			switch (face)
			{
			case 0: // +X
				mat = Matrix4(
					0.0f, 0.0f, -1.0f, 0.0f,
					0.0f, -1.0f, 0.0f, 0.0f,
					-1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
				break;
			case 1: // -X
				mat = Matrix4(
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, -1.0f, 0.0f, 0.0f,
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
				break;
			case 2: // +Y
				mat = Matrix4(
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, -1.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
				break;
			case 3: // -Y
				mat = Matrix4(
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, -1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
				break;
			case 4: // +Z
				mat = Matrix4(
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, -1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, -1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
				break;
			case 5: // -Z
				mat = Matrix4(
					-1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, -1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
				break;
			}
			mat.Translate(-from);
			return mat;
		}
		Matrix4 ViewMatrix(const Vector3& dir, const Vector3& up, const Vector3& pos)
		{
			//      |	s[0]   s[1]   s[2]  0  |
			//      |	u[0]   u[1]   u[2]  0  |
			//  M = |  -f[0]  -f[1]  -f[2]  0  |
			//      |	 0	    0      0    1  |

			Vector3 s = dir ^ up;
			Matrix4 mat(
				s.x, up.x, -dir.x, 0.0f,
				s.y, up.y, -dir.y, 0.0f,
				s.z, up.z, -dir.z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
			mat.Translate(-pos);

			return mat;
		}
		Matrix4 ViewMatrix(const Matrix3& rot, const Vector3& pos)
		{
			//      |	s[0]   s[1]   s[2]  0  |
			//      |	u[0]   u[1]   u[2]  0  |
			//  M = |  -f[0]  -f[1]  -f[2]  0  |
			//      |	 0	    0      0    1  |

			Matrix4 mat(
				rot.a[2][0], rot.a[1][0], -rot.a[0][0], 0.0f,
				rot.a[2][1], rot.a[1][1], -rot.a[0][1], 0.0f,
				rot.a[2][2], rot.a[1][2], -rot.a[0][2], 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
			mat.Translate(-pos);

			return mat;
		}
		Matrix4 ViewHorizontalMirrorMatrix(const Matrix3& rot, const Vector3& pos, float h)
		{
			//      |	s[0]   s[1]   s[2]  0  |
			//      |	u[0]   u[1]   u[2]  0  |
			//  M = |  -f[0]  -f[1]  -f[2]  0  |
			//      |	 0	    0      0    1  |
			float mh = pos.y - 2.0f*h;
			Matrix4 mat(
				rot.a[2][0], rot.a[1][0], -rot.a[0][0], 0.0f,
				-rot.a[2][1], -rot.a[1][1], rot.a[0][1], 0.0f,
				rot.a[2][2], rot.a[1][2], -rot.a[0][2], 0.0f,
				-pos.x*rot.a[2][0] - mh*rot.a[2][1] - pos.z*rot.a[2][2],
				-pos.x*rot.a[1][0] - mh*rot.a[1][1] - pos.z*rot.a[1][2],
				pos.x*rot.a[0][0] + mh*rot.a[0][1] + pos.z*rot.a[0][2],
				1.0f);
			return mat;
		}
		Matrix3 RotationMatrix(const Vector3& dir)
		{
			Vector3 up, s;
			if (fabs(dir.y) < 1.0f)
			{
				s = (dir ^ UNIT_Y).GetNormalized();
				up = s ^ dir;
			}
			else // dir == unit y
			{
				s = UNIT_Z;
				up = -UNIT_X;
			}
			Matrix3 mat(
				dir.x, dir.y, dir.z,
				up.x, up.y, up.z,
				s.x, s.y, s.z);

			return mat;
		}
		Matrix4 RotationMatrix(const Vector3& dir, const Vector3& up)
		{
			//      |	f[0]   u[0]   c[0]  0  |
			//      |	f[1]   u[1]   c[1]  0  |
			//  M = |   f[2]   u[2]   c[2]  0  |
			//      |	 0	    0      0    1  |
			Vector3 s = dir ^ up;
			Matrix4 mat(
				dir.x, dir.y, dir.z, 0.0f,
				up.x, up.y, up.z, 0.0f,
				s.x, s.y, s.z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);

			return mat;
		}
		Matrix4 RotationMatrix(const Matrix3& m)
		{
			Matrix4 mat(
				m.a[0][0], m.a[0][1], m.a[0][2], 0.0f,
				m.a[1][0], m.a[1][1], m.a[1][2], 0.0f,
				m.a[2][0], m.a[2][1], m.a[2][2], 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
			return mat;
		}
		Matrix4 OrientationMatrix(const Vector3& dir, const Vector3& up, const Vector3& pos)
		{
			//      |	f[0]   u[0]   c[0]  0  |
			//      |	f[1]   u[1]   c[1]  0  |
			//  M = |   f[2]   u[2]   c[2]  0  |
			//      |	 0	    0      0    1  |
			Vector3 s = dir ^ up;
			Matrix4 mat(
				dir.x, dir.y, dir.z, 0.0f,
				up.x, up.y, up.z, 0.0f,
				s.x, s.y, s.z, 0.0f,
				pos.x, pos.y, pos.z, 1.0f);

			return mat;
		}
		Matrix4 OrientationMatrix(const Matrix3& m, const Vector3& pos)
		{
			Matrix4 mat(
				m.a[0][0], m.a[0][1], m.a[0][2], 0.0f,
				m.a[1][0], m.a[1][1], m.a[1][2], 0.0f,
				m.a[2][0], m.a[2][1], m.a[2][2], 0.0f,
				pos.x, pos.y, pos.z, 1.0f);
			return mat;
		}
		Matrix3 Identity3(void)
		{
			Matrix3 mat(
				1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f);

			return mat;
		}
		Matrix4 Identity4(void)
		{
			Matrix4 mat(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);

			return mat;
		}
		Matrix4 Translate(float x, float y, float z)
		{
			Matrix4 mat(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				x, y, z, 1.0f);

			return mat;
		}
		Matrix4 Translate(const Vector3& v)
		{
			Matrix4 mat(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				v.x, v.y, v.z, 1.0f);

			return mat;
		}
		Matrix3 Scale3(float x)
		{
			Matrix3 mat(
				x, 0.0f, 0.0f,
				0.0f, x, 0.0f,
				0.0f, 0.0f, x);

			return mat;
		}
		Matrix3 Scale3(float x, float y, float z)
		{
			Matrix3 mat(
				x, 0.0f, 0.0f,
				0.0f, y, 0.0f,
				0.0f, 0.0f, z);

			return mat;
		}
		Matrix4 Scale4(float x)
		{
			Matrix4 mat(
				x, 0.0f, 0.0f, 0.0f,
				0.0f, x, 0.0f, 0.0f,
				0.0f, 0.0f, x, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);

			return mat;
		}
		Matrix4 Scale4(float x, float y, float z)
		{
			Matrix4 mat(
				x, 0.0f, 0.0f, 0.0f,
				0.0f, y, 0.0f, 0.0f,
				0.0f, 0.0f, z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);

			return mat;
		}
		Matrix3 Rotate3(float c, float s, float x, float y, float z)
		{
			//      |	xx(1-c)+c	xy(1-c)-zs  xz(1-c)+ys	 0  |
			//      |	yx(1-c)+zs	yy(1-c)+c   yz(1-c)-xs	 0  |
			//  M = |   xz(1-c)-ys	yz(1-c)+xs  zz(1-c)+c	 0  |
			//      |	     0	        0		   0	     1  |
			// abs(x,y,z) must be equal 1
			Matrix3 mat(
				x*x*(1.0f - c) + c, y*x*(1.0f - c) + z*s, x*z*(1.0f - c) - y*s,
				x*y*(1.0f - c) - z*s, y*y*(1.0f - c) + c, y*z*(1.0f - c) + x*s,
				x*z*(1.0f - c) + y*s, y*z*(1.0f - c) - x*s, z*z*(1.0f - c) + c);

			return mat;
		}
		Matrix4 Rotate4(float c, float s, float x, float y, float z)
		{
			//      |	xx(1-c)+c	xy(1-c)-zs  xz(1-c)+ys	 0  |
			//      |	yx(1-c)+zs	yy(1-c)+c   yz(1-c)-xs	 0  |
			//  M = |   xz(1-c)-ys	yz(1-c)+xs  zz(1-c)+c	 0  |
			//      |	     0	        0		   0	     1  |
			// abs(x,y,z) must be equal 1
			Matrix4 mat(
				x*x*(1.0f - c) + c, y*x*(1.0f - c) + z*s, x*z*(1.0f - c) - y*s, 0.0f,
				x*y*(1.0f - c) - z*s, y*y*(1.0f - c) + c, y*z*(1.0f - c) + x*s, 0.0f,
				x*z*(1.0f - c) + y*s, y*z*(1.0f - c) - x*s, z*z*(1.0f - c) + c, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);

			return mat;
		}
        Matrix3 NormalMatrix(const Matrix4& modelview)
        {
            Matrix4 mat = modelview.GetInverse().GetTransposed();
            return Matrix3(mat.e11, mat.e12, mat.e13, mat.e21, mat.e22, mat.e23, mat.e31, mat.e32, mat.e33);
        }
		void TransformVectorByQuaternion(Vector3& res, const Vector3& v, const Quaternion& q)
		{
			res = (q * Quaternion(v.x, v.y, v.z, 0.0f) * q.GetInverse()).xyz();
		}
		void MatrixToQuaternion(Quaternion &q, const Matrix3 &m)
		{
			float trace = m.a[0][0] + m.a[1][1] + m.a[2][2];
			if (trace > 0.0f)
			{
				float s = 0.5f / sqrtf(trace + 1.0f);
				q.w = 0.25f / s;
				q.x = (m.a[1][2] - m.a[2][1]) * s;
				q.y = (m.a[2][0] - m.a[0][2]) * s;
				q.z = (m.a[0][1] - m.a[1][0]) * s;
			}
			else if (m.a[0][0] > m.a[1][1] && m.a[0][0] > m.a[2][2])
			{
				float s = 2.0f * sqrtf(1.0f + m.a[0][0] - m.a[1][1] - m.a[2][2]);
				q.w = (m.a[1][2] - m.a[2][1]) / s;
				q.x = 0.25f * s;
				q.y = (m.a[1][0] + m.a[0][1]) / s;
				q.z = (m.a[2][0] + m.a[0][2]) / s;
			}
			else if (m.a[1][1] > m.a[2][2])
			{
				float s = 2.0f * sqrtf(1.0f + m.a[1][1] - m.a[0][0] - m.a[2][2]);
				q.w = (m.a[2][0] - m.a[0][2]) / s;
				q.x = (m.a[1][0] + m.a[0][1]) / s;
				q.y = 0.25f * s;
				q.z = (m.a[2][1] + m.a[1][2]) / s;
			}
			else
			{
				float s = 2.0f * sqrtf(1.0f + m.a[2][2] - m.a[0][0] - m.a[1][1]);
				q.w = (m.a[0][1] - m.a[1][0]) / s;
				q.x = (m.a[2][0] + m.a[0][2]) / s;
				q.y = (m.a[2][1] + m.a[1][2]) / s;
				q.z = 0.25f * s;
			}
		}
		void QuaternionToMatrix(Matrix3 &m, const Quaternion &q)
		{
			float x2, y2, z2, xx, yy, zz, xy, xz, yz, wx, wy, wz;
			x2 = q.x + q.x;
			y2 = q.y + q.y;
			z2 = q.z + q.z;
			xx = q.x * x2;  xy = q.x * y2;  xz = q.x * z2;
			yy = q.y * y2;  yz = q.y * z2;  zz = q.z * z2;
			wx = q.w * x2;  wy = q.w * y2;  wz = q.w * z2;

			m.a[0][0] = 1.0f - (yy + zz);
			m.a[0][1] = xy + wz;
			m.a[0][2] = xz - wy;

			m.a[1][0] = xy - wz;
			m.a[1][1] = 1.0f - (xx + zz);
			m.a[1][2] = yz + wx;

			m.a[2][0] = xz + wy;
			m.a[2][1] = yz - wx;
			m.a[2][2] = 1.0f - (xx + yy);
		}
		void QuaternionSLERP(Quaternion &q, const Quaternion& q1, const Quaternion& q2, float t)
		{
			float new1[4];
			float cos_om, scale0, scale1;
			cos_om = q1 & q2;
			if (cos_om < 0.0f)
			{
				cos_om = -cos_om;
				new1[0] = -q2.x; new1[1] = -q2.y;
				new1[2] = -q2.z; new1[3] = -q2.w;
			}
			else
			{
				new1[0] = q2.x; new1[1] = q2.y;
				new1[2] = q2.z; new1[3] = q2.w;
			}
			if (cos_om > 0.9999f)
			{
				float omega = acos(cos_om);
				float sin_om = sin(omega);
				scale0 = sin((1.0f - t) * omega) / sin_om;
				scale1 = sin(t * omega) / sin_om;
			}
			else // small angle - LERP
			{
				scale0 = 1.0f - t;
				scale1 = t;
			}
			q.x = scale0 * q1.x + scale1 * new1[0];
			q.y = scale0 * q1.y + scale1 * new1[1];
			q.z = scale0 * q1.z + scale1 * new1[2];
			q.w = scale0 * q1.w + scale1 * new1[3];
		}
		void WorldToScreen(const Vector4& world, const Matrix4& proj, const Matrix4& view,
        	const Vector4 viewport, Vector2& screen)
		{
			// Step 1: 4d Clip space
			vec4 pos_clip = proj * view * world;

			// Step 2: 3d Normalized Device Coordinate space
			vec3 pos_ndc = pos_clip.xyz() / pos_clip.w;

			// Step 3: Window Coordinate space
			screen.x = (pos_ndc.x + 1.0f) * 0.5f * viewport.z + viewport.x;
			screen.y = (pos_ndc.y + 1.0f) * 0.5f * viewport.w + viewport.y;
		}
		float DistanceToCamera(const Vector3& world, const Matrix4& view)
		{
			vec4 pos_eye = view * Vector4(world, 1.0f);
			return pos_eye.xyz().Length();
		}
		float DistanceToCamera(const Vector4& world, const Matrix4& view)
		{
			vec4 pos_eye = view * world;
			return pos_eye.xyz().Length();
		}
		void ScreenToRay(const Vector3& screen_ndc, const Matrix4& proj, const Matrix4& view, Vector3& ray)
		{
			// Step 1: 3d Normalised Device Coordinates ( range [-1:1, -1:1, -1:1] )
			// We already have it in screen_ndc input variable.
		
			// Step 2: 4d Homogeneous Clip Coordinates ( range [-1:1, -1:1, -1:1, -1:1] )
			vec4 ray_clip(
				screen_ndc.x,
				screen_ndc.y,
				-1.0, // We want our ray's z to point forwards - this is usually the negative z direction in OpenGL style.
				1.0
				);
			// Step 3: 4d Eye (Camera) Coordinates ( range [-x:x, -y:y, -z:z, -w:w] )
			vec4 ray_eye = proj.GetInverse() * ray_clip;
			// Now, we only needed to un-project the x,y part, so let's manually set the z,w part to mean "forwards, and not a point".
			ray_eye.z = -1.0f;
			ray_eye.w = 0.0f;
			// Step 4: 4d World Coordinates ( range [-x:x, -y:y, -z:z, -w:w] )
			ray = (view.GetInverse() * ray_eye).xyz();
			ray.Normalize();
		}

	} // namespace math
} // namespace sht