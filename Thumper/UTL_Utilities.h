/*
Author: Nathan Dunn
Module: UTL

Multipurpose utilitarian data structures.

Known bugs and limitations:
- N/A

*/

#pragma once

namespace UTL
{
	const float pi = 3.14159265f;

	/*
	Math implementing this class follows the right hand rule convention.
	Roll is rotation around x axis in body frame of reference, 0 aligns with the XY plane.
	Pitch is rotation around y axis in body frame of reference, 0 aligns with the XY plane.
	Yaw is rotation around z axis in body frame of reference, 0 is the XZ plane.
	Use radians.
	*/
	struct bodyCenteredAttitude
	{
		float roll = 0; 
		float pitch = 0; 
		float yaw = 0; 
		float xScale = 1;
		float yScale = 1;
		float zScale = 1;
	};

	struct vec3f
	{
		float x = 0;
		float y = 0;
		float z = 0;
	};

	void ApplyBodyTranslationToGlobal(UTL::bodyCenteredAttitude& attitude, UTL::vec3f& globalPosition, UTL::vec3f& displacementInBodyFrame) noexcept;

	/*
	This matrix utility math was written while I was learning linear algebra, so there a lot of comments explaining how linear algebra works.
	
	I will use row major storage for everything.

	That means that a matrix represents rows in contiguous memory.
	m = R1C1 R1C2 R1C3
		R2C1 R2C2 R2C3
		R3C1 R3C2 R3C3
	becomes
	m = {R1C1, R1C2, R1C3, R2C1, R2C2, R2C3, R3C1, R3C2, R3C3}

	Vectors don't have multiple columns, so they have one value for each row.
	v = R1C1
		R2C1
		R3C1
	becomes
	v = {R1C1, R2C1, R3C1}

	First application: transforming between RHR and LHR coordinate systems.
	RHR is defined in a matrix operation as xAxis x yAxis = zAxis
	LHR is defined in a matrix operation as xAxis x yAxis = -zAxis
	This stems from the cross product being defined mathematically as a right hand rule operation.
	To convert, multiple the z axis by -1.
	To do this in a matrix, we multiply the vector by a matrix that negates z (r3c1):
	v_rotated = v * transform =	r1c1		1	0	0	=	r1c1
								r2c1	*	0	1	0		r2c1
								r3c1		0	0	-1		-r3c1
	That is implemented in convertBetweenHandedness().

	Second application: a rotation matrix.
	Rotations can apply to vectors, and to other matrices.
	A rotation of a vector is rotating the vector around it's tail end (note the distinction from a line segmant that is two points).
	A rotation of a matrix is just rotating all the vectors in the matrix with the same rotation.
	Matrix rotations are not commutative.
	M1*M2 != M2*M1
	R*M rotates M around R

	*/

	struct vector3f
	{
		float r1c1 = 0;
		float r2c1 = 0;
		float r3c1 = 0;
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

	void ConvertBetweenHandedness(UTL::vector3f& v) noexcept;
	matrix3x3f Multiply(matrix3x3f& m1, matrix3x3f& m2) noexcept;
}