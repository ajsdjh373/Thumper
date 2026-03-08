/*
Author: Nathan Dunn
Module: G3D

See associated header file for more information
*/

#include <d3dcompiler.h>
#include "G3D_Shader_WireFrame.h"
#include "ERR_ErrorEngine.h"

/*
Creates the necessary D3D11 resources for all shaders required for wire frame rendering.

Arguments:
re		The render engine to use

Return:
N/A

Safeties and known issues:
- N/A

*/
G3D::Shader_WireFrame::Shader_WireFrame(G3D::RenderEngine& re)
{
		// vertex shader
	ERR::errorTracker.TestHR(D3DReadFileToBlob(L"WireFrameVS.cso", &pBlob), __FILE__, __func__, __LINE__);
	// I am leaving out a potentially useful shader feature in the next call https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/storing-variables-and-types-for-shaders-to-share
	ERR::errorTracker.TestHR(re.GetDevice()->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pVertexShader), __FILE__, __func__, __LINE__);

	// input layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> IED =
	{
		{ "Position",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	0,					D3D11_INPUT_PER_VERTEX_DATA,	0 }
	};
	ERR::errorTracker.TestHR(re.GetDevice()->CreateInputLayout(&IED[0], (UINT)IED.size(), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout), __FILE__, __func__, __LINE__);

	pBlob.Reset();

	// pixel shader
	ERR::errorTracker.TestHR(D3DReadFileToBlob(L"WireFramePS.cso", &pBlob), __FILE__, __func__, __LINE__);
	ERR::errorTracker.TestHR(re.GetDevice()->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pPixelShader), __FILE__, __func__, __LINE__);
}

/*
Per frame D3D11 operations for all shaders required for wire frame rendering.

Arguments:
re		The render engine to use

Return:
N/A

Safeties and known issues:
- N/A

*/
void G3D::Shader_WireFrame::Draw(G3D::RenderEngine& re)
{
	// vertex shader
	re.GetImmediateContext()->VSSetShader(pVertexShader.Get(), NULL, 0u);

	// input layout
	re.GetImmediateContext()->IASetInputLayout(pInputLayout.Get());

	// pixel shader
	re.GetImmediateContext()->PSSetShader(pPixelShader.Get(), NULL, 0u);
}