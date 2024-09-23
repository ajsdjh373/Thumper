/*
Author: Nathan Dunn
Module: G3D

Wrapper for the Texture shader pair (pixel + vertex shaders)
Instantiate only one instance of this type to be optimal

Known bugs and limitations:
- More than one instance of this type is possible and won't break anything, but gives no extra functionality

*/

#pragma once
#include "G3D_RenderEngine.h"
#include "G3D_Texture2D.h"

namespace G3D
{
	class Shader_Texture;
}

class G3D::Shader_Texture
{
public:
	// public member functions
	Shader_Texture() = delete;
	Shader_Texture(G3D::RenderEngine RE, G3D::Texture2D& TextureResource);
	void Create();
	void Draw();

private:
	// private member functions

public:
	// public member variables

	// return holder for D3D11 API calls
	// replace this when error handling is implemented
	HRESULT HR;

private:
	// private member variables
	bool CreationComplete;
	G3D::RenderEngine& RE;
	G3D::Texture2D& TextureResource;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;

};