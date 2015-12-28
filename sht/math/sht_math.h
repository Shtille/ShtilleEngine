#pragma once
#ifndef __SHT_MATH_MATH_H__
#define __SHT_MATH_MATH_H__

// Interface

#include "vector.h"
#include "matrix.h"
#include "quaternion.h"

namespace sht {
	namespace math {
        
        // Math constants
        const float kPi     = 3.1415926535f;
        const float kTwoPi  = 6.2831853071f;

		//! Vector orthogonalization via Gram-Shmidt method
		Vector3 Orthogonalize(const Vector3& v1, const Vector3& v2);

		// Projection matrices
		Matrix4 OrthoMatrix(float left, float right, float bottom, float top, float zNear, float zFar);
		Matrix4 PerspectiveMatrix(float fov, int width, int height, float zNear, float zFar); // NOTE: fov in degrees!
		// Camera matrices
		Matrix4 LookAt(const Vector3& from, const Vector3& at);
		Matrix4 LookAtCube(const Vector3& from, int face);
		Matrix4 ViewMatrix(const Vector3& dir, const Vector3& up, const Vector3& pos);
		Matrix4 ViewMatrix(const Matrix3& rot, const Vector3& pos);
		Matrix4 ViewHorizontalMirrorMatrix(const Matrix3& rot, const Vector3& pos, float h);
		// Object transformation matrices
		Matrix3 RotationMatrix(const Vector3& dir);
		Matrix4 RotationMatrix(const Vector3& dir, const Vector3& up);
		Matrix4 RotationMatrix(const Matrix3& m);
		Matrix4 OrientationMatrix(const Vector3& dir, const Vector3& up, const Vector3& pos);
		Matrix4 OrientationMatrix(const Matrix3& m, const Vector3& pos);
		// Other matrix generation functions
		Matrix3 Identity3(void);
		Matrix4 Identity4(void);
		Matrix4 Translate(float x, float y, float z);
		Matrix4 Translate(const Vector3& v);
		Matrix3 Scale3(float x);
		Matrix3 Scale3(float x, float y, float z);
		Matrix4 Scale4(float x);
		Matrix4 Scale4(float x, float y, float z);
		Matrix3 Rotate3(float c, float s, float x, float y, float z);
		Matrix4 Rotate4(float c, float s, float x, float y, float z);
        
        Matrix3 NormalMatrix(const Matrix4& modelview);

		void TransformVectorByQuaternion(Vector3& res, const Vector3& v, const Quaternion& q);
		void MatrixToQuaternion(Quaternion &q, const Matrix3 &m);
		void QuaternionToMatrix(Matrix3 &m, const Quaternion &q);
		void QuaternionSLERP(Quaternion &q, const Quaternion& q1, const Quaternion& q2, float t);

		void WorldToScreen(const Vector4& world, const Matrix4& proj, const Matrix4& view, const Vector4 viewport, Vector2& screen);
	    float DistanceToCamera(const Vector3& world, const Matrix4& view);
    	float DistanceToCamera(const Vector4& world, const Matrix4& view);
    	void ScreenToRay(const Vector3& screen_ndc, const Matrix4& proj, const Matrix4& view, Vector3& ray);

	} // namespace sht
} // namespace math

#endif