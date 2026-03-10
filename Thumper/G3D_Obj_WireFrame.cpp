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
G3D::Obj_WireFrame::Obj_WireFrame(std::vector<UTL::vector3f>& vertices, std::vector<unsigned short>& indices, G3D::RenderEngine& re)
{
	// copy input data into a local cache
	verticesCache = vertices;
	indicesCache = indices;

	// vertex buffer
	D3D11_BUFFER_DESC VBD = {};
	VBD.ByteWidth = (UINT)(sizeof(UTL::vector3f) * verticesCache.size());
	VBD.Usage = D3D11_USAGE_DYNAMIC;
	VBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VBD.MiscFlags = 0u;
	VBD.StructureByteStride = sizeof(UTL::vector3f);

	D3D11_SUBRESOURCE_DATA VSD = {};
	VSD.pSysMem = &verticesCache[0];
	VSD.SysMemPitch = 0; // unused
	VSD.SysMemSlicePitch = 0; // unused

	ERR::errorTracker.TestHR(re.GetDevice()->CreateBuffer(&VBD, &VSD, &pVertexBuffer), __FILE__, __func__, __LINE__);

	// index buffer
	D3D11_BUFFER_DESC IBD = {};
	IBD.ByteWidth = (UINT)(sizeof(unsigned short) * indicesCache.size());
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
	CBD.ByteWidth = sizeof(G3D::shaderConstantBuffer);
	CBD.Usage = D3D11_USAGE_DYNAMIC;
	CBD.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CBD.MiscFlags = 0;
	CBD.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA CSD = {};
	CSD.pSysMem = &shaderConstantBuffer;
	CSD.SysMemPitch = 0; // unused
	CSD.SysMemSlicePitch = 0; // unused

	ERR::errorTracker.TestHR(re.GetDevice()->CreateBuffer(&CBD, &CSD, &pConstantBuffer), __FILE__, __func__, __LINE__);

	// transform buffer init to default values
	UTL::vector3f tempAttitude{ 0, 0, 0};
	UTL::vector3f tempPosition{ 0, 0, 0 };
	UTL::vector3f tempScale{ 1, 1, 1 };
	UpdateBodyAndGlobalFrame(tempAttitude, tempPosition, tempScale);

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
ERR::ErrorCodes G3D::Obj_WireFrame::Draw(G3D::RenderEngine& re) 
{
	// constant buffer
	shaderConstantBuffer.scale = scale;
	UTL::vector3f v;
	v = UTL::Negate(re.camera.position);
	shaderConstantBuffer.translation = UTL::Add(position, v);

	// incorrect, just using q1 was last known state. Need to do this after translation
	// I need to pass two rotation matrices to the shader - one for the object's frame, one for the camera's frame
	// scale > body frame rotation > translate > camera frame rotation > projection matrix
	// the inversion may be right, though
	//UTL::vector4f q1 = UTL::QuaternionFromEuler(attitude);
	//UTL::vector4f q2 = UTL::QuaternionFromEuler(re.camera.attitude);
	//q2 = { q2.r1c1, -q2.r2c1, -q2.r3c1, -q2.r4c1 };
	//UTL::vector4f q3 = UTL::QuaternionMultiply(q2, q1);
	//UTL::matrix3x3f rotationMatrix = UTL::RotationFromQuaternion(q3);
	
	UTL::vector4f q = UTL::QuaternionFromEuler(attitude);
	UTL::matrix3x3f rotationMatrix = UTL::RotationFromQuaternion(q);
	shaderConstantBuffer.rotation_bodyFrame[0] = rotationMatrix.r1c1;
	shaderConstantBuffer.rotation_bodyFrame[1] = rotationMatrix.r1c2;
	shaderConstantBuffer.rotation_bodyFrame[2] = rotationMatrix.r1c3;
	shaderConstantBuffer.rotation_bodyFrame[3] = 0;
	shaderConstantBuffer.rotation_bodyFrame[4] = rotationMatrix.r2c1;
	shaderConstantBuffer.rotation_bodyFrame[5] = rotationMatrix.r2c2;
	shaderConstantBuffer.rotation_bodyFrame[6] = rotationMatrix.r2c3;
	shaderConstantBuffer.rotation_bodyFrame[7] = 0;
	shaderConstantBuffer.rotation_bodyFrame[8] = rotationMatrix.r3c1;
	shaderConstantBuffer.rotation_bodyFrame[9] = rotationMatrix.r3c2;
	shaderConstantBuffer.rotation_bodyFrame[10] = rotationMatrix.r3c3;
	shaderConstantBuffer.rotation_bodyFrame[11] = 0;
	q = UTL::QuaternionFromEuler(re.camera.attitude);
	rotationMatrix = UTL::RotationFromQuaternion(q);
	shaderConstantBuffer.rotation_cameraFrame[0] = rotationMatrix.r1c1;
	shaderConstantBuffer.rotation_cameraFrame[1] = rotationMatrix.r1c2;
	shaderConstantBuffer.rotation_cameraFrame[2] = rotationMatrix.r1c3;
	shaderConstantBuffer.rotation_cameraFrame[3] = 0;
	shaderConstantBuffer.rotation_cameraFrame[4] = rotationMatrix.r2c1;
	shaderConstantBuffer.rotation_cameraFrame[5] = rotationMatrix.r2c2;
	shaderConstantBuffer.rotation_cameraFrame[6] = rotationMatrix.r2c3;
	shaderConstantBuffer.rotation_cameraFrame[7] = 0;
	shaderConstantBuffer.rotation_cameraFrame[8] = rotationMatrix.r3c1;
	shaderConstantBuffer.rotation_cameraFrame[9] = rotationMatrix.r3c2;
	shaderConstantBuffer.rotation_cameraFrame[10] = rotationMatrix.r3c3;
	shaderConstantBuffer.rotation_cameraFrame[11] = 0;
	shaderConstantBuffer.ar = re.GetWidth() / re.GetHeight();
	shaderConstantBuffer.ft = tan(re.camera.fov / 2);
	shaderConstantBuffer.a = re.camera.farPlane / (re.camera.farPlane - re.camera.nearPlane);
	shaderConstantBuffer.b = re.camera.farPlane * re.camera.nearPlane / (re.camera.farPlane - re.camera.nearPlane);

	D3D11_MAPPED_SUBRESOURCE MSR;
	
	// note that map and unmap ask for a Subresource, this is the index if multiple were input in the Set function for that resource
	ERR::errorTracker.TestHR(re.GetImmediateContext()->Map(pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &MSR), __FILE__, __func__, __LINE__);
	memcpy(MSR.pData, &shaderConstantBuffer, sizeof(G3D::shaderConstantBuffer));
	re.GetImmediateContext()->Unmap(pConstantBuffer.Get(), 0u);
	
	// vertex buffer
	const unsigned int VertexSize = sizeof(UTL::vector3f);
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
Accepts the global position and attitude. Applies attitude first.
Do not pass deltas, pass final positions.
*/
void G3D::Obj_WireFrame::UpdateBodyAndGlobalFrame(UTL::vector3f& attitude, UTL::vector3f& position, UTL::vector3f& scale)
{
	this->attitude = attitude;
	this->position = position;
	this->scale = scale;
}
