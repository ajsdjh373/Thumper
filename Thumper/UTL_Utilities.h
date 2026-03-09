/*
Author: Nathan Dunn
Module: UTL

Multipurpose utilitarian data structures.

All the math in this library follows these conventions:
-RHR coordinates
-Quaternions store in WXYZ format
-attitude is stored in RPY format
-Roll is rotation around x axis in body frame of reference, 0 aligns with the XY plane
-Pitch is rotation around y axis in body frame of reference, 0 aligns with the XY plane
-Yaw is rotation around z axis in body frame of reference, 0 is the XZ plane
-Radians for angles
-Matrices are row major (column vectors)

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

	void ConvertBetweenHandedness(UTL::vector3f& v) noexcept;
	vector3f Add(vector3f& v1, vector3f& v2) noexcept; // good
	matrix3x3f Multiply(matrix3x3f& m1, matrix3x3f& m2) noexcept;
	matrix4x4f Multiply(matrix4x4f& m1, matrix4x4f& m2) noexcept;
	vector3f Multiply(matrix3x3f& m1, vector3f& v1) noexcept;
	vector4f Multiply(matrix4x4f& m1, vector4f& v1) noexcept;
	vector3f Multiply(matrix3x3f& m, float scalar) noexcept;
	vector3f Negate(vector3f& v) noexcept; // good
	vector4f QuaternionMultiply(vector4f& q1, vector4f& q2) noexcept;
	vector4f QuaternionFromEuler(const vector3f& attitude) noexcept; // good
	matrix3x3f RotationFromQuaternion(vector4f& q) noexcept; // good
	matrix4x4f TransformFromRotation(matrix4x4f& rotation, vector3f& position) noexcept;
	matrix4x4f ScaleFromVector(vector3f& scale) noexcept;
	matrix4x4f TranslationFromVector(vector3f& position) noexcept;
	void Transpose(matrix3x3f& m) noexcept;
	void Transpose(matrix4x4f& m) noexcept;
}