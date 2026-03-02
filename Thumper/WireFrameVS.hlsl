/*
Author: Nathan Dunn
Module: G3D

Vertex shader for wire frame rendering.
Applies the model transform (scale, rotation, translation) from the
constant buffer to each vertex position.

Constant buffer slot 0: transform matrix (transposed XMMATRIX from CPU)
*/

cbuffer CBuf : register(b0)
{
	matrix transform;
};

float4 main(float3 pos : Position) : SV_Position
{
	return mul(float4(pos, 1.0f), transform);
}
