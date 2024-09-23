/*
Author: Nathan Dunn
Module: G3D

See associated header file for more information
*/

#include <d3dcompiler.h>
#include "G3D_Shader_Texture.h"

/*
Constructor

Inputs:
RE		The render engine that owns the immediate context this shader will use

Outputs:
N/A

*/
G3D::Shader_Texture::Shader_Texture(G3D::RenderEngine RE, G3D::Texture2D& TextureResource) :
	CreationComplete{ false },
	RE{ RE },
	TextureResource{ TextureResource }
{}

/*
Call this to create the D3D11 resources necessary to for this set of shaders
It is safe to call Create multiple times, but only the first does anything

Inputs:
N/A

Outputs:
N/A

*/
void G3D::Shader_Texture::Create()
{
	// protection against duplicating D3D11 resources and losing the old pointers, thus causing a memory leak
	if (CreationComplete) return;

	// vertex shader
	HR = D3DReadFileToBlob(L"TextureVS.cso", &pBlob);
	// I am leaving out a potentially useful shader feature in the next call https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/storing-variables-and-types-for-shaders-to-share
	HR = RE.GetDevice()->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pVertexShader);

	// input layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> IED =
	{
		{ "Position",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	0,					D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "TexCoord",	0,	DXGI_FORMAT_R32G32_FLOAT,		0,	sizeof(float) * 3,	D3D11_INPUT_PER_VERTEX_DATA,	0 }
	};
	HR = RE.GetDevice()->CreateInputLayout(&IED[0], IED.size(), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout);

	// nothing for textures, only obligations to textures is in Draw

	// sampler, technically could be moved to a different instance to avoid duplication for slightly better performance
	D3D11_SAMPLER_DESC SamplerDesc = {};
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	HR = RE.GetDevice()->CreateSamplerState(&SamplerDesc, &pSampler);

	// pixel shader
	HR = D3DReadFileToBlob(L"TexturePS.cso", &pBlob);
	HR = RE.GetDevice()->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pPixelShader);

	// creation successful
	CreationComplete = true;
}

/*
Call this to perform all per-frame operations for this shader

Inputs:
N/A

Outputs:
N/A

*/
void G3D::Shader_Texture::Draw()
{
	// vertex shader
	RE.GetImmediateContext()->VSSetShader(pVertexShader.Get(), NULL, 0u);

	// input layout
	RE.GetImmediateContext()->IASetInputLayout(pInputLayout.Get());

	// sampler
	RE.GetImmediateContext()->PSSetSamplers(0, 1, pSampler.GetAddressOf());

	// texture
	TextureResource.Draw();

	// pixel shader
	RE.GetImmediateContext()->PSSetShader(pPixelShader.Get(), NULL, 0u);
}

