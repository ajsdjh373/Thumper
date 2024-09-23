/*
Author: Nathan Dunn
Module: G3D

See associated header file for more information
*/

#include <Windows.h>
#include <math.h>
#include "G3D_Obj_Tile.h"

/*
Constructor

Inputs:
hWnd		handle for the window that this will render to
Width		width of the output window in pixels
Height		height of the output window in pixels

Outputs:
N/A

*/
G3D::Obj_Tile::Obj_Tile(G3D::RenderEngine& RE, G3D::Shader_Texture& Shader, G3D::TileGeometry Layout, float Yaw, float XPosition, float YPosition) :
	RE{ RE },
	Layout{ Layout },
	Shader{ Shader },
	Vertices{ {} },
	Indices{ {} },
	XPosition{ XPosition },
	YPosition{ YPosition },
	Yaw{ Yaw },
	FlagDraw{ false },
	CreationComplete{ false },
	OptionalsNeedMapped{ false }
{
	BuildGeometry();
}

/*
Call this to create the D3D11 resources necessary to for this set of shaders.
It is safe to call Create multiple times, but only the first does anything.
This will do nothing if the vertex and index buffers are empty, which might
happen if BuildGeometry has an error.

Inputs:
N/A

Outputs:
N/A

*/
void G3D::Obj_Tile::Create()
{
	if (CreationComplete) return;
	if (Vertices.size() < 1) return;
	if (Indices.size() < 1) return;

	// vertex buffer
	D3D11_BUFFER_DESC VBD = {};
	VBD.ByteWidth = sizeof(G3D::Vertex_PT) * Vertices.size();
	VBD.Usage = D3D11_USAGE_DYNAMIC;
	VBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VBD.MiscFlags = 0u;
	VBD.StructureByteStride = sizeof(G3D::Vertex_PT);

	D3D11_SUBRESOURCE_DATA VSD = {};
	VSD.pSysMem = &Vertices[0];
	VSD.SysMemPitch = 0; // unused
	VSD.SysMemSlicePitch = 0; // unused

	HR = RE.GetDevice()->CreateBuffer(&VBD, &VSD, &pVertexBuffer);

	// index buffer
	D3D11_BUFFER_DESC IBD = {};
	IBD.ByteWidth = sizeof(unsigned short) * Indices.size();
	IBD.Usage = D3D11_USAGE_DYNAMIC;
	IBD.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	IBD.MiscFlags = 0;
	IBD.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA ISD = {};
	ISD.pSysMem = &Indices[0];
	ISD.SysMemPitch = 0; // unused
	ISD.SysMemSlicePitch = 0; // unused

	HR = RE.GetDevice()->CreateBuffer(&IBD, &ISD, &pIndexBuffer);

	// shaders
	Shader.Create();

	// topology - no resources are created for the topology, so the only requirement is in Draw

	// constant buffers
	D3D11_BUFFER_DESC CBD = {};
	CBD.ByteWidth = sizeof(DirectX::XMMATRIX);
	CBD.Usage = D3D11_USAGE_DYNAMIC;
	CBD.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CBD.MiscFlags = 0;
	CBD.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA CSD = {};
	CSD.pSysMem = &Transform;
	CSD.SysMemPitch = 0; // unused
	CSD.SysMemSlicePitch = 0; // unused

	HR = RE.GetDevice()->CreateBuffer(&CBD, &CSD, &pConstantBuffer);

	CreationComplete = true;
	FlagDraw = true;
}

/*
Call this to perform all per-frame operations for this resource
Does nothing if Create has not completed successfully.

Inputs:
N/A

Outputs:
N/A

*/
void G3D::Obj_Tile::Draw()
{
	if (!CreationComplete) return;
	if (!FlagDraw) return;

	// update the constant buffer transform - ideally this is off loaded to the GPU later
	CalculateTransform();
	Map();

	// vertex buffer
	const unsigned int VertexSize = sizeof(G3D::Vertex_PT);
	RE.GetImmediateContext()->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &VertexSize, 0u);

	// index buffer
	RE.GetImmediateContext()->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	// shaders
	Shader.Draw();

	// topology
	RE.GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// constant buffers
	RE.GetImmediateContext()->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

}

/*
Call this to modify the z values of vertices for the tile.
This does nothing if Index would be out of range for the vector.

Inputs:
Index	Index of the vertex that should have it's z updated
Z		The new z value

Outputs:
N/A

*/
void G3D::Obj_Tile::SetVertexZ(int Index, float Z)
{
	if (Index >= Vertices.size()) return;
	Vertices[Index].Z = Z;
	OptionalsNeedMapped = true;
}

/*
Call this to set the yaw of the tile.

Inputs:
NewYaw	The new yaw value, in degrees

Outputs:
N/A

*/
void G3D::Obj_Tile::SetYaw(float NewYaw)
{
	Yaw = NewYaw;
	// don't need to set OptionalsNeedMapped because Yaw is accounted for via Transform
}

/*
Call this to calculate the new transform matrix.
This method accounts for the camera's affect on the view.

Inputs:
N/A

Outputs:
N/A

*/
void G3D::Obj_Tile::CalculateTransform()
{
	Transform = DirectX::XMMatrixRotationRollPitchYaw(0, 0, Yaw * 3.1415f / 180) * DirectX::XMMatrixTranslation(XPosition, YPosition, 0);
	Transform = Transform * RE.Cam.GetMatrix();
}

/*
Call this to write state variables to existing DirectX resources.
This function does nothing if Create has not completed.

Inputs:
N/A

Outputs:
N/A

*/
void G3D::Obj_Tile::Map()
{
	if (!CreationComplete) return;
	if (Vertices.size() < 1) return;
	if (Indices.size() < 1) return;

	D3D11_MAPPED_SUBRESOURCE MSR;

	// map the required element: the transform buffer
	// note that map and unmap ask for an Subresource, this is the index if multiple were input in the Set function for that resource
	HR = RE.GetImmediateContext()->Map(pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &MSR);
	memcpy(MSR.pData, &Transform, sizeof(DirectX::XMMATRIX));
	RE.GetImmediateContext()->Unmap(pConstantBuffer.Get(), 0u);

	// check the optionals flag and proceed accordingly
	if (!OptionalsNeedMapped) return;

	// remapping the vertex buffer
	HR = RE.GetImmediateContext()->Map(pVertexBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &MSR);
	memcpy(MSR.pData, &Vertices[0], sizeof(G3D::Vertex_PT) * Vertices.size());
	RE.GetImmediateContext()->Unmap(pVertexBuffer.Get(), 0u);

}

/*
Call this to populate the vertex and index buffers based on the selected tile geometry.
Consectutive calls to this are NOT safe because if the vertex or index buffer grows
and Map is called, a memory overflow will occur.

Inputs:
N/A

Outputs:
N/A

*/
void G3D::Obj_Tile::BuildGeometry()
{
	switch (Layout)
	{
	case(G3D::TileGeometry::Null):
		Vertices = {};
		Indices = {};
		break;

	case(G3D::TileGeometry::Single):
		Vertices = {
			//					XPos	YPos	ZPos	XUV		YUV
			G3D::Vertex_PT(-0.5f,	0.5f,	0.0f,	0.0f,	1.0f),
			G3D::Vertex_PT(0.5f,	0.5f,	0.0f,	1.0f,	1.0f),
			G3D::Vertex_PT(-0.5f,	-0.5f,	0.0f,	0.0f,	0.0f),
			G3D::Vertex_PT(0.5f,	-0.5f,	0.0f,	1.0f,	0.0f)
		};
		Indices = {
			/*
			X	Y	Z
			*/
			0,	2,	1,
			1,	2,	3
		};
		break;

	case(G3D::TileGeometry::Double):
		Vertices = {
			//					XPos	YPos	ZPos	XUV		YUV
			G3D::Vertex_PT(-0.5f,	0.5f,	0.0f,	0.0f,	1.0f),
			G3D::Vertex_PT(0.5f,	0.5f,	0.0f,	1.0f,	1.0f),
			G3D::Vertex_PT(-0.5f,	-0.5f,	0.0f,	0.0f,	0.0f),
			G3D::Vertex_PT(0.5f,	-0.5f,	0.0f,	1.0f,	0.0f),
			G3D::Vertex_PT(0.0f,	0.0f,	0.0f,	0.5f,	0.5f)
		};
		Indices = {
			/*
			X	Y	Z
			*/
			0,	2,	4,
			4,	2,	3,
			3,	1,	4,
			4,	1,	0
		};
		break;

	case(G3D::TileGeometry::TransitionA):
		Vertices = {
			//					XPos	YPos	ZPos	XUV		YUV
			G3D::Vertex_PT(-0.5f,	0.5f,	0.0f,	0.0f,	1.0f),
			G3D::Vertex_PT(0.5f,	0.5f,	0.0f,	1.0f,	1.0f),
			G3D::Vertex_PT(-0.5f,	-0.5f,	0.0f,	0.0f,	0.0f),
			G3D::Vertex_PT(0.5f,	-0.5f,	0.0f,	1.0f,	0.0f),
			G3D::Vertex_PT(0.0f,	0.0f,	0.0f,	0.5f,	0.5f),
			G3D::Vertex_PT(0.0f,	-0.5f,	0.0f,	0.5f,	0.0f)
		};
		Indices = {
			/*
			X	Y	Z
			*/
			0,	2,	4,
			4,	2,	5,
			5,	3,	4,
			4,	3,	1,
			1,	0,	4
		};
		break;

	case(G3D::TileGeometry::Star):
		Vertices = {
			//					XPos	YPos	ZPos	XUV		YUV
			G3D::Vertex_PT(-0.5f,	0.5f,	0.0f,	0.0f,	1.0f),
			G3D::Vertex_PT(0.5f,	0.5f,	0.0f,	1.0f,	1.0f),
			G3D::Vertex_PT(-0.5f,	-0.5f,	0.0f,	0.0f,	0.0f),
			G3D::Vertex_PT(0.5f,	-0.5f,	0.0f,	1.0f,	0.0f),
			G3D::Vertex_PT(0.0f,	0.0f,	0.0f,	0.5f,	0.5f),
			G3D::Vertex_PT(0.0f,	-0.5f,	0.0f,	0.5f,	0.0f),
			G3D::Vertex_PT(0.5f,	0.0f,	0.0f,	1.0f,	0.5f),
			G3D::Vertex_PT(0.0f,	0.5f,	0.0f,	0.5f,	1.0f),
			G3D::Vertex_PT(-0.5f,	0.0f,	0.0f,	0.0f,	0.5f)
		};
		Indices = {
			/*
			X	Y	Z
			*/
			4,	2,	5,
			4,	5,	3,
			4,	3,	6,
			4,	6,	1,
			4,	1,	7,
			4,	7,	0,
			4,	0,	8,
			4,	8,	2
		};
		break;

	case(G3D::TileGeometry::Triple):
		Vertices = {
			//					XPos	YPos	ZPos	XUV		YUV
			G3D::Vertex_PT(-0.5f,	0.5f,	0.0f,	0.0f,	1.0f),
			G3D::Vertex_PT(0.5f,	0.5f,	0.0f,	1.0f,	1.0f),
			G3D::Vertex_PT(-0.5f,	-0.5f,	0.0f,	0.0f,	0.0f),
			G3D::Vertex_PT(0.5f,	-0.5f,	0.0f,	1.0f,	0.0f),
			G3D::Vertex_PT(0.0f,	0.0f,	0.0f,	0.5f,	0.5f),
			G3D::Vertex_PT(0.0f,	-0.5f,	0.0f,	0.5f,	0.0f),
			G3D::Vertex_PT(0.5f,	0.0f,	0.0f,	1.0f,	0.5f),
			G3D::Vertex_PT(0.0f,	0.5f,	0.0f,	0.5f,	1.0f),
			G3D::Vertex_PT(-0.5f,	0.0f,	0.0f,	0.0f,	0.5f)
		};
		Indices = {
			/*
			X	Y	Z
			*/
			0,	8,	7,
			7,	8,	4,
			4,	6,	7,
			7,	6,	1,
			2,	5,	8,
			8,	5,	4,
			4,	5,	6,
			6,	5,	3
		};
		break;

	default:
		Vertices = {};
		Indices = {};
		break;
	}
}