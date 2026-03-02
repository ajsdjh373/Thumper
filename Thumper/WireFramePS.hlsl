/*
Author: Nathan Dunn
Module: G3D

Pixel shader for wire frame rendering.
Outputs a solid white color for all fragments. The wire frame appearance
itself is produced by the D3D11_FILL_WIREFRAME rasterizer state set in
the render engine, not by this shader.

To change the wire frame color, modify the float4 return value.
Format is (R, G, B, A) with each channel in the range [0.0, 1.0].
*/

float4 main() : SV_Target
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
