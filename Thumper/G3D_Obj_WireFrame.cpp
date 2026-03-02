/*
Author: Nathan Dunn
Module: G3D

See associated header file for more information
*/

#include "G3D_Obj_WireFrame.h"

/*
This constructor creates the D3D11 resources for this object.

Inputs:
vertices	geometry vertices
indices		indices for triangles
re			renderEngine to use for D3D11 API calls

Return:
- N/A

Safeties and known issues:
- Not doing anything with bad HRESULTS

*/
G3D::Obj_WireFrame::Obj_WireFrame(std::vector<UTL::point>& vertices, std::vector<unsigned short>& indices, G3D::RenderEngine& re)
{
	// copy input data into a local cache
	verticesCache = vertices;
	indicesCache = indices;

	// vertex buffer
	D3D11_BUFFER_DESC VBD = {};
	VBD.ByteWidth = sizeof(UTL::point) * verticesCache.size();
	VBD.Usage = D3D11_USAGE_DYNAMIC;
	VBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VBD.MiscFlags = 0u;
	VBD.StructureByteStride = sizeof(UTL::point);

	D3D11_SUBRESOURCE_DATA VSD = {};
	VSD.pSysMem = &verticesCache[0];
	VSD.SysMemPitch = 0; // unused
	VSD.SysMemSlicePitch = 0; // unused

	ERR::errorTracker.TestHR(re.GetDevice()->CreateBuffer(&VBD, &VSD, &pVertexBuffer), __FILE__, __func__, __LINE__);

	// index buffer
	D3D11_BUFFER_DESC IBD = {};
	IBD.ByteWidth = sizeof(unsigned short) * indicesCache.size();
	IBD.Usage = D3D11_USAGE_DYNAMIC;
	IBD.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	IBD.MiscFlags = 0;
	IBD.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA ISD = {};
	ISD.pSysMem = &indicesCache[0];
	ISD.SysMemPitch = 0; // unused
	ISD.SysMemSlicePitch = 0; // unused

	ERR::errorTracker.TestHR(re.GetDevice()->CreateBuffer(&IBD, &ISD, &pIndexBuffer), __FILE__, __func__, __LINE__);

	// create shaders here

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

	ERR::errorTracker.TestHR(re.GetDevice()->CreateBuffer(&CBD, &CSD, &pConstantBuffer), __FILE__, __func__, __LINE__);

	// transform buffer init to default values
	UTL::attitude tempAttitude{ 0, 0, 0, 0, 0, 0, 1, 1, 1 };
	UpdateAttitude(tempAttitude);

}

/*
Draws the object to the screen. Must be called once per frame. Call after update functions to render updates in the next frame.

Inputs:
re			renderEngine to use for D3D11 API calls

Return:
- N/A

Safeties and known issues:
- Not doing anything with bad HRESULTS

*/
ERR::ErrorCodes G3D::Obj_WireFrame::Draw(G3D::RenderEngine& re) {

	// combine model transform with current camera matrix
	DirectX::XMMATRIX transform1 = Transform * re.camera.GetMatrix(re.GetWidth(), re.GetHeight());

	// transpose to match expected shader layout
	DirectX::XMMATRIX transform2 = DirectX::XMMatrixTranspose(transform1);

	// mapping the constant buffer
	D3D11_MAPPED_SUBRESOURCE MSR;

	// map the transform buffer
	// note that map and unmap ask for an Subresource, this is the index if multiple were input in the Set function for that resource
	ERR::errorTracker.TestHR(re.GetImmediateContext()->Map(pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &MSR), __FILE__, __func__, __LINE__);
	memcpy(MSR.pData, &transform2, sizeof(DirectX::XMMATRIX));
	re.GetImmediateContext()->Unmap(pConstantBuffer.Get(), 0u);

	// vertex buffer
	const unsigned int VertexSize = sizeof(UTL::point);
	const UINT offset = 0u;
	re.GetImmediateContext()->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &VertexSize, &offset);

	// index buffer
	re.GetImmediateContext()->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	// topology
	re.GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// constant buffers
	re.GetImmediateContext()->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

	// tell D3D11 to draw it on screen
	re.GetImmediateContext()->DrawIndexed((UINT)indicesCache.size(), 0u, 0);

	return ERR::ErrorCodes::okay;
}

/*
Description of the function and its intended use

Arguments:
attitude		attitude to update to

Return:
- N/A

Safeties and known issues:
- N/A

*/
void G3D::Obj_WireFrame::UpdateAttitude(UTL::attitude& attitude)
{
	/*
	NOTE: operating in RHR
	*/
	Transform = DirectX::XMMatrixScaling(attitude.xScale, attitude.yScale, attitude.zScale) * DirectX::XMMatrixRotationRollPitchYaw(attitude.roll, attitude.pitch, attitude.yaw)
		* DirectX::XMMatrixTranslation(attitude.x, attitude.y, attitude.z);
}