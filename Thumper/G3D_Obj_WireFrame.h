/*
Author: Nathan Dunn
Module: G3D

Current optimizations:
- indexed geometry

Known bugs and limitations:
- N/A

*/

#pragma once
#include <vector>
#include <DirectXMath.h>
#include "G3D_RenderEngine.h"
#include "ERR_ErrorEngine.h"
#include "UTL_Utilities.h"

namespace G3D
{
	class Obj_WireFrame;

	struct shaderConstantBuffer
	{
		// refer to wire WireFrameVS.hlsl for the meaning of each variable
		UTL::vector3f translation = { 0, 0, 0 };
		float ft = 0;
		float rotation_bodyFrame[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		float rotation_cameraFrame[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		UTL::vector3f scale = { 0, 0, 0 };
		float ar = 0;
		float a = 0;
		float b = 0;
		float pack[2] = { 0, 0 };
	};
}

class G3D::Obj_WireFrame
{
public:
	Obj_WireFrame() = delete;
	Obj_WireFrame(std::vector<UTL::vector3f>& vertices, std::vector<unsigned short>& indices, G3D::RenderEngine& re);
	//~Obj_WireFrame();
	ERR::ErrorCodes Draw(G3D::RenderEngine& re);
	void UpdateBodyAndGlobalFrame(UTL::vector3f& attitude, UTL::vector3f& position, UTL::vector3f& scale);
	//void CopyRotationMatrix(UTL::matrix4x4f& output) noexcept;
	//void CopyTranslationMatrix(UTL::matrix4x4f& output) noexcept;

private:

public:
	UTL::vector3f attitude;
	UTL::vector3f position;
	UTL::vector3f scale;

private:
	std::vector<UTL::vector3f> verticesCache;
	std::vector<unsigned short> indicesCache;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	shaderConstantBuffer shaderConstantBuffer;
	

};
