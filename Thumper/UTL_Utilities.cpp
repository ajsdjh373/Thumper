/*
Author: Nathan Dunn
Module: UTL

See associated header file for more information
*/

#include "UTL_Utilities.h"

/*
This function applies a displacement vector in the body frame to the global position. Use it to move things with controls in the body frame.

Arguments:
attitude					global orientation of the body that is getting displaced
global						global position of the body that is getting displaced
displacementInBodyFrame		displacement vector in the body frame of reference

Return:
a		description

Safeties and known issues:
- a
- b
- c

*/
void UTL::ApplyBodyTranslationToGlobal(UTL::bodyCenteredAttitude& attitude, UTL::vec3f& globalPosition, UTL::vec3f& displacementInBodyFrame) noexcept
{

}

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
