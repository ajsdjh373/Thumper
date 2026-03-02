/*
Author: Nathan Dunn
Module: G3D

Wrapper for the wire frame shader pair (pixel + vertex shaders)
Instantiate only one instance of this type to be optimal
Must be initialized in the RenderEngine constructor

Known bugs and limitations:
- More than one instance of this type is possible and won't break anything, but gives no extra functionality

*/

#pragma once
#include "G3D_RenderEngine.h"

namespace G3D
{
	class Shader_WireFrame;
}

class G3D::Shader_WireFrame
{
public:
	// public member functions
	Shader_WireFrame() = delete;
	Shader_WireFrame(G3D::RenderEngine& re);
	void Draw(G3D::RenderEngine& re);

private:
	// private member functions

public:
	// public member variables

private:
	// private member variables
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;

};