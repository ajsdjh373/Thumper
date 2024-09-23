/*
Author: Nathan Dunn
Module: G3D

Texture, for both D3D resource and the data itself
The texture data is bound to this resource because the D3D resource is meant to not be duplicated

Known bugs and limitations:
- N/A

*/

#pragma once
#include <vector>
#include "G3D_RenderEngine.h"
#include "G3D_Utilities.h"

namespace G3D
{
	class Texture2D;
}

class G3D::Texture2D
{
public:
	// public member functions
	Texture2D() = delete;
	Texture2D(G3D::RenderEngine RE, const std::vector<G3D::Color_ARGB>& Buffer, const unsigned int Width);
	void Create();
	void Draw();
	void Empty();

private:
	// private member functions

public:
	// public member variables
	HRESULT HR;

private:
	// private member variables
	std::vector<G3D::Color_ARGB> Image;
	unsigned int ImageWidth;
	unsigned int ImageHeight;
	G3D::RenderEngine RE;
	bool CreationComplete;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;

};