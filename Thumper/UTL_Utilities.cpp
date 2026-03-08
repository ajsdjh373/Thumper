/*
Author: Nathan Dunn
Module: UTL

See associated header file for more information
*/

#include <cmath>
#include "UTL_Utilities.h"

/*
See the header file for more information.
*/
void UTL::ConvertBetweenHandedness(UTL::vector3f& v) noexcept 
{
	v.r3c1 = -v.r3c1;
}

/*
m1 * m2 = m3, m3 returned.
*/
UTL::matrix3x3f UTL::Multiply(matrix3x3f& m1, matrix3x3f& m2) noexcept
{
	UTL::matrix3x3f m3;
	// rull of thumb: in RC notation the inside indices remain constant and equal to the destination rc indices while the outside indices are iterated on from 1 to n
	// for M3=M1*M2, M3​[i,j] = ∑ from k=1 to n​ (M1​[i,k]⋅M2​[k,j]) where i and j and the row and column indices
	m3.r1c1 = m1.r1c1 * m2.r1c1 + m1.r1c2 * m2.r2c1 + m1.r1c3 * m2.r3c1;
	m3.r1c2 = m1.r1c1 * m2.r1c2 + m1.r1c2 * m2.r2c2 + m1.r1c3 * m2.r3c2;
	m3.r1c3 = m1.r1c1 * m2.r1c3 + m1.r1c2 * m2.r2c3 + m1.r1c3 * m2.r3c3;

	m3.r2c1 = m1.r2c1 * m2.r1c1 + m1.r2c2 * m2.r2c1 + m1.r2c3 * m2.r3c1;
	m3.r2c2 = m1.r2c1 * m2.r1c2 + m1.r2c2 * m2.r2c2 + m1.r2c3 * m2.r3c2;
	m3.r2c3 = m1.r2c1 * m2.r1c3 + m1.r2c2 * m2.r2c3 + m1.r2c3 * m2.r3c3;

	m3.r3c1 = m1.r3c1 * m2.r1c1 + m1.r3c2 * m2.r2c1 + m1.r3c3 * m2.r3c1;
	m3.r3c2 = m1.r3c1 * m2.r1c2 + m1.r3c2 * m2.r2c2 + m1.r3c3 * m2.r3c2;
	m3.r3c3 = m1.r3c1 * m2.r1c3 + m1.r3c2 * m2.r2c3 + m1.r3c3 * m2.r3c3;

	return m3;
}

/*
m1 * m2 = m3, m3 returned.
*/
UTL::matrix4x4f UTL::Multiply(matrix4x4f& m1, matrix4x4f& m2) noexcept
{
	UTL::matrix4x4f m3;
	// rull of thumb: in RC notation the inside indices remain constant and equal to the destination rc indices while the outside indices are iterated on from 1 to n
	// for M3=M1*M2, M3​[i,j] = ∑ from k=1 to n​ (M1​[i,k]⋅M2​[k,j]) where i and j and the row and column indices
	m3.r1c1 = m1.r1c1 * m2.r1c1 + m1.r1c2 * m2.r2c1 + m1.r1c3 * m2.r3c1 + m1.r1c4 * m2.r4c1;
	m3.r1c2 = m1.r1c1 * m2.r1c2 + m1.r1c2 * m2.r2c2 + m1.r1c3 * m2.r3c2 + m1.r1c4 * m2.r4c2;
	m3.r1c3 = m1.r1c1 * m2.r1c3 + m1.r1c2 * m2.r2c3 + m1.r1c3 * m2.r3c3 + m1.r1c4 * m2.r4c3;
	m3.r1c4 = m1.r1c1 * m2.r1c4 + m1.r1c2 * m2.r2c4 + m1.r1c3 * m2.r3c4 + m1.r1c4 * m2.r4c4;

	m3.r2c1 = m1.r2c1 * m2.r1c1 + m1.r2c2 * m2.r2c1 + m1.r2c3 * m2.r3c1 + m1.r2c4 * m2.r4c1;
	m3.r2c2 = m1.r2c1 * m2.r1c2 + m1.r2c2 * m2.r2c2 + m1.r2c3 * m2.r3c2 + m1.r2c4 * m2.r4c2;
	m3.r2c3 = m1.r2c1 * m2.r1c3 + m1.r2c2 * m2.r2c3 + m1.r2c3 * m2.r3c3 + m1.r2c4 * m2.r4c3;
	m3.r2c4 = m1.r2c1 * m2.r1c4 + m1.r2c2 * m2.r2c4 + m1.r2c3 * m2.r3c4 + m1.r2c4 * m2.r4c4;

	m3.r3c1 = m1.r3c1 * m2.r1c1 + m1.r3c2 * m2.r2c1 + m1.r3c3 * m2.r3c1 + m1.r3c4 * m2.r4c1;
	m3.r3c2 = m1.r3c1 * m2.r1c2 + m1.r3c2 * m2.r2c2 + m1.r3c3 * m2.r3c2 + m1.r3c4 * m2.r4c2;
	m3.r3c3 = m1.r3c1 * m2.r1c3 + m1.r3c2 * m2.r2c3 + m1.r3c3 * m2.r3c3 + m1.r3c4 * m2.r4c3;
	m3.r3c4 = m1.r3c1 * m2.r1c4 + m1.r3c2 * m2.r2c4 + m1.r3c3 * m2.r3c4 + m1.r3c4 * m2.r4c4;

	m3.r4c1 = m1.r4c1 * m2.r1c1 + m1.r4c2 * m2.r2c1 + m1.r4c3 * m2.r3c1 + m1.r4c4 * m2.r4c1;
	m3.r4c2 = m1.r4c1 * m2.r1c2 + m1.r4c2 * m2.r2c2 + m1.r4c3 * m2.r3c2 + m1.r4c4 * m2.r4c2;
	m3.r4c3 = m1.r4c1 * m2.r1c3 + m1.r4c2 * m2.r2c3 + m1.r4c3 * m2.r3c3 + m1.r4c4 * m2.r4c3;
	m3.r4c4 = m1.r4c1 * m2.r1c4 + m1.r4c2 * m2.r2c4 + m1.r4c3 * m2.r3c4 + m1.r4c4 * m2.r4c4;

	return m3;
}

/*
v2 = m1 * v1, v2 returned.
*/
UTL::vector3f UTL::Multiply(matrix3x3f& m1, vector3f& v1) noexcept
{
	// for M3=M1*M2, M3​[i,j] = ∑ from k=1 to n​ (M1​[i,k]⋅M2​[k,j]) where i and j and the row and column indices
	vector3f v2;
	v2.r1c1 = m1.r1c1 * v1.r1c1 + m1.r1c2 * v1.r2c1 + m1.r1c3 * v1.r3c1;
	v2.r2c1 = m1.r2c1 * v1.r1c1 + m1.r2c2 * v1.r2c1 + m1.r2c3 * v1.r3c1;
	v2.r3c1 = m1.r3c1 * v1.r1c1 + m1.r3c2 * v1.r2c1 + m1.r3c3 * v1.r3c1;

	return v2;
}

/*
v2 = m1 * v1, v2 returned.
*/
UTL::vector4f UTL::Multiply(matrix4x4f& m1, vector4f& v1) noexcept
{
	// for M3=M1*M2, M3​[i,j] = ∑ from k=1 to n​ (M1​[i,k]⋅M2​[k,j]) where i and j and the row and column indices
	vector4f v2;
	v2.r1c1 = m1.r1c1 * v1.r1c1 + m1.r1c2 * v1.r2c1 + m1.r1c3 * v1.r3c1 + m1.r1c4 * v1.r4c1;
	v2.r2c1 = m1.r2c1 * v1.r1c1 + m1.r2c2 * v1.r2c1 + m1.r2c3 * v1.r3c1 + m1.r2c4 * v1.r4c1;
	v2.r3c1 = m1.r3c1 * v1.r1c1 + m1.r3c2 * v1.r2c1 + m1.r3c3 * v1.r3c1 + m1.r3c4 * v1.r4c1;
	v2.r4c1 = m1.r4c1 * v1.r1c1 + m1.r4c2 * v1.r2c1 + m1.r4c3 * v1.r3c1 + m1.r4c4 * v1.r4c1;

	return v2;
}

/*
v2 = v * scalar, v2 returned
*/
UTL::vector3f UTL::Multiply(matrix3x3f& m, float scalar) noexcept
{
	vector3f v2;
	v2.r1c1 = m.r1c1 * scalar;
	v2.r2c1 = m.r2c1 * scalar;
	v2.r3c1 = m.r3c1 * scalar;

	return v2;
}

/*
Multiplies two quaternions together. q1*q2=q3, q3 returned.
*/
UTL::vector4f UTL::QuaternionMultiply(vector4f& q1, vector4f& q2) noexcept
{
	vector4f q3;
	q3.r1c1 = q1.r1c1 * q2.r1c1 - q1.r2c1 * q2.r2c1 - q1.r3c1 * q2.r3c1 - q1.r4c1 * q2.r4c1;
	q3.r2c1 = q1.r1c1 * q2.r2c1 + q1.r2c1 * q2.r1c1 + q1.r3c1 * q2.r4c1 - q1.r4c1 * q2.r3c1;
	q3.r3c1 = q1.r1c1 * q2.r3c1 - q1.r2c1 * q2.r4c1 + q1.r3c1 * q2.r1c1 + q1.r4c1 * q2.r2c1;
	q3.r4c1 = q1.r1c1 * q2.r4c1 + q1.r2c1 * q2.r3c1 - q1.r3c1 * q2.r2c1 + q1.r4c1 * q2.r1c1;

	return q3;
}

/*
Rotates yaw, then pitch, then roll around the reference axis.
*/
UTL::vector4f UTL::QuaternionFromEuler(const vector3f& attitude) noexcept
{
	vector4f qz; // yaw rotates around z
	qz.r1c1 = std::cos(attitude.r3c1 / 2);
	qz.r2c1 = 0;
	qz.r3c1 = 0;
	qz.r4c1 = std::sin(attitude.r3c1 / 2);

	vector4f qy; // pitch rotates around y
	qy.r1c1 = std::cos(attitude.r2c1 / 2);
	qy.r2c1 = 0;
	qy.r3c1 = std::sin(attitude.r2c1 / 2);
	qy.r4c1 = 0;

	vector4f qx; // roll rotates around x
	qx.r1c1 = std::cos(attitude.r1c1 / 2);
	qx.r2c1 = std::sin(attitude.r1c1 / 2);
	qx.r3c1 = 0;
	qx.r4c1 = 0;

	vector4f q = QuaternionMultiply(qx, qy);
	q = QuaternionMultiply(q, qz);

	return q;
}

/*
Returns a rotation matrix. This only converts a quaternion to a 4x4 matrix. It does not apply a rotation.
*/
UTL::matrix4x4f UTL::RotationFromQuaternion(vector4f& q) noexcept
{
	// normalize first to protect against drift
	float mag = std::sqrt(q.r1c1 * q.r1c1 + q.r2c1 * q.r2c1 + q.r3c1 * q.r3c1 + q.r4c1 * q.r4c1);
	vector4f normalizedq = q;
	normalizedq.r1c1 /= mag;
	normalizedq.r2c1 /= mag;
	normalizedq.r3c1 /= mag;
	normalizedq.r4c1 /= mag;

	matrix4x4f m;
	m.r1c1 = 1 - 2 * (normalizedq.r3c1 * normalizedq.r3c1 + normalizedq.r4c1 * normalizedq.r4c1);
	m.r1c2 = 2 * (normalizedq.r2c1 * normalizedq.r3c1 - normalizedq.r1c1 * normalizedq.r4c1);
	m.r1c3 = 2 * (normalizedq.r2c1 * normalizedq.r4c1 + normalizedq.r1c1 * normalizedq.r3c1);
	m.r1c4 = 0;
	m.r2c1 = 2 * (normalizedq.r2c1 * normalizedq.r3c1 + normalizedq.r1c1 * normalizedq.r4c1);
	m.r2c2 = 1 - 2 * (normalizedq.r2c1 * normalizedq.r2c1 + normalizedq.r4c1 * normalizedq.r4c1);
	m.r2c3 = 2 * (normalizedq.r3c1 * normalizedq.r4c1 - normalizedq.r1c1 * normalizedq.r2c1);
	m.r2c4 = 0;
	m.r3c1 = 2 * (normalizedq.r2c1 * normalizedq.r4c1 - normalizedq.r1c1 * normalizedq.r3c1);
	m.r3c2 = 2 * (normalizedq.r3c1 * normalizedq.r4c1 + normalizedq.r1c1 * normalizedq.r2c1);
	m.r3c3 = 1 - 2 * (normalizedq.r2c1 * normalizedq.r2c1 + normalizedq.r3c1 * normalizedq.r3c1);
	m.r3c4 = 0;
	m.r4c1 = 0;
	m.r4c2 = 0;
	m.r4c3 = 0;
	m.r4c4 = 1;

	return m;
}

/*
Combines the rotation matrix with the translation vector, and returns the result (the transform matrix).
The translation vector needs to be in XYZ format.
Multiply the transform matrix by a vector to transform the vector.
If the vector has W=0, it applies only the rotation. If W=1, it applies the rotation and translation.
*/
UTL::matrix4x4f UTL::TransformFromRotation(matrix4x4f& rotation, vector3f& position) noexcept
{
	UTL::matrix4x4f m2;
	m2 = rotation;
	m2.r1c4 = position.r1c1;
	m2.r2c4 = position.r2c1;
	m2.r3c4 = position.r3c1;

	return m2;
}

/*
Generates the scale matrix from the scale vector.
Use a uniform vector or this will break rotations.
*/
UTL::matrix4x4f UTL::ScaleFromVector(vector3f& scale) noexcept
{
	UTL::matrix4x4f m;
	m.r1c1 = scale.r1c1;
	m.r1c2 = 0;
	m.r1c3 = 0;
	m.r1c4 = 0;
	m.r2c1 = 0;
	m.r2c2 = scale.r2c1;
	m.r2c3 = 0;
	m.r2c4 = 0;
	m.r3c1 = 0;
	m.r3c2 = 0;
	m.r3c3 = scale.r3c1;
	m.r3c4 = 0;
	m.r4c1 = 0;
	m.r4c2 = 0;
	m.r4c3 = 0;
	m.r4c4 = 1;

	return m;
}

/*
Generates the translation matrix from the position vector.
*/
UTL::matrix4x4f UTL::TranslationFromVector(vector3f& position) noexcept
{
	matrix4x4f m;
	m.r1c1 = 1;
	m.r1c2 = 0;
	m.r1c3 = 0;
	m.r1c4 = position.r1c1;
	m.r2c1 = 0;
	m.r2c2 = 1;
	m.r2c3 = 0;
	m.r2c4 = position.r2c1;
	m.r3c1 = 0;
	m.r3c2 = 0;
	m.r3c3 = 1;
	m.r3c4 = position.r3c1;
	m.r4c1 = 0;
	m.r4c2 = 0;
	m.r4c3 = 0;
	m.r4c4 = 1;

	return m;
}

/*
Transposes the input matrix in-place.
*/
void UTL::Transpose(matrix3x3f& m) noexcept
{
	float temp;
	temp = m.r2c1;
	m.r2c1 = m.r1c2;
	m.r1c2 = temp;
	temp = m.r3c1;
	m.r3c1 = m.r1c3;
	m.r1c3 = temp;
	temp = m.r3c2;
	m.r3c2 = m.r2c3;
	m.r2c3 = temp;
}

/*
Tranposes the input matrix in-place
*/
void UTL::Transpose(matrix4x4f& m) noexcept
{
	float temp;
	temp = m.r1c2;
	m.r1c2 = m.r2c1;
	m.r2c1 = temp;
	temp = m.r1c3;
	m.r1c3 = m.r3c1;
	m.r3c1 = temp;
	temp = m.r1c4;
	m.r1c4 = m.r4c1;
	m.r4c1 = temp;
	temp = m.r2c3;
	m.r2c3 = m.r3c2;
	m.r3c2 = temp;
	temp = m.r2c4;
	m.r2c4 = m.r4c2;
	m.r4c2 = temp;
	temp = m.r3c4;
	m.r3c4 = m.r4c3;
	m.r4c3 = temp;
}
