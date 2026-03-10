/*
Author: Nathan Dunn
Module: UTL

Multipurpose utilitarian data structures.

All the math in this library follows these conventions:
-Quaternions store in WXYZ format
-attitude is stored in RPY format
-Roll is rotation around x axis, 0 aligns with the XY plane
-Pitch is rotation around y axis, 0 aligns with the XY plane
-Yaw is rotation around z axis, 0 is the XZ plane
-Radians for angles
-Matrices are row major (column vectors)
-All functions implement right handed operations - although most vector operations don't depend on handedness. Note that the shader is where the internal handedness convention is converted into the screenspace.

Known bugs and limitations:
- N/A

*/

#pragma once

namespace UTL
{
	const float pi = 3.14159265f;

	struct vector3f
	{
		float r1c1 = 0;
		float r2c1 = 0;
		float r3c1 = 0;
	};

	struct vector4f
	{
		float r1c1 = 0;
		float r2c1 = 0;
		float r3c1 = 0;
		float r4c1 = 0;
	};

	struct matrix3x3f
	{
		float r1c1 = 0;
		float r1c2 = 0;
		float r1c3 = 0;
		float r2c1 = 0;
		float r2c2 = 0;
		float r2c3 = 0;
		float r3c1 = 0;
		float r3c2 = 0;
		float r3c3 = 0;
	};

	struct matrix4x4f
	{
		float r1c1 = 0;
		float r1c2 = 0;
		float r1c3 = 0;
		float r1c4 = 0;
		float r2c1 = 0;
		float r2c2 = 0;
		float r2c3 = 0;
		float r2c4 = 0;
		float r3c1 = 0;
		float r3c2 = 0;
		float r3c3 = 0;
		float r3c4 = 0;
		float r4c1 = 0;
		float r4c2 = 0;
		float r4c3 = 0;
		float r4c4 = 0;
	};

	//void ConvertBetweenHandedness(UTL::vector3f& v) noexcept;
	vector3f Add(const vector3f& v1, const vector3f& v2) noexcept; // good
	matrix3x3f Multiply(const matrix3x3f& m1, const matrix3x3f& m2) noexcept;
	matrix4x4f Multiply(const matrix4x4f& m1, const matrix4x4f& m2) noexcept;
	vector3f Multiply(const matrix3x3f& m1, const vector3f& v1) noexcept;
	vector4f Multiply(const matrix4x4f& m1, const vector4f& v1) noexcept;
	vector3f Multiply(const matrix3x3f& m, const float scalar) noexcept;
	vector3f Negate(const vector3f& v) noexcept; // good
	vector4f QuaternionMultiply(const vector4f& q1, const vector4f& q2) noexcept;
	vector4f QuaternionFromEuler(const vector3f& attitude) noexcept; // good
	vector3f UnitVectorFromEuler(const vector3f& attitude) noexcept;
	vector3f RotateVectorByInverseEuler(const vector3f& attitude, const vector3f vector) noexcept;
	matrix3x3f RotationFromQuaternion(const vector4f& q) noexcept; // good

}