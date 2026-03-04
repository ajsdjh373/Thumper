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
}

class G3D::Obj_WireFrame
{
public:
	Obj_WireFrame() = delete;
	Obj_WireFrame(std::vector<UTL::point>& vertices, std::vector<unsigned short>& indices, G3D::RenderEngine& re);
	//~Obj_WireFrame();
	ERR::ErrorCodes Draw(G3D::RenderEngine& re);
	void UpdateBodyAndGlobalFrame(UTL::bodyCenteredAttitude& bodyCenteredAttitude, UTL::globalFrame& globalFrame);

private:

public:

private:
	std::vector<UTL::point> verticesCache;
	std::vector<unsigned short> indicesCache;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	DirectX::XMMATRIX Transform;

};
