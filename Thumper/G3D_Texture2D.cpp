/*
Author: Nathan Dunn
Module: G3D

See associated header file for more information
*/

#include <Windows.h>
#include "G3D_Texture2D.h"

/*
Constructor
If a buffer is provided with a length that is not a multiple of width, then it will be
truncated to the largest possible multiple of width.

Inputs:
RE		The render engine that owns the immediate context this shader will use
Buffer	The array of texels that will be used for this texture
Width	The width of the texture in texels

Outputs:
N/A

*/
G3D::Texture2D::Texture2D(G3D::RenderEngine RE, const std::vector<G3D::Color_ARGB>& Buffer, const unsigned int Width) :
	RE{ RE },
	Image{ Buffer },
	ImageWidth{ Width },
	CreationComplete{ false }
{
	// calculate the height and truncate the buffer if necessary
	ImageHeight = Image.size() / ImageWidth;
	if (ImageWidth * ImageHeight < Image.size())
	{
		Image.resize(ImageWidth * ImageHeight);
		Image.shrink_to_fit();
	}

}

/*
Call this to create the D3D11 resources necessary to for this set of shaders.
It is safe to call Create multiple times, but only the first does anything.
This does nothing if the image buffer is empty.

Inputs:
N/A

Outputs:
N/A

*/
void G3D::Texture2D::Create()
{
	// protection against duplicating D3D11 resources and losing the old pointers, thus causing a memory leak
	if (CreationComplete) return;
	if (Image.size() < 1) return;

	D3D11_TEXTURE2D_DESC TextureDesc = {};
	TextureDesc.Width = ImageWidth;
	TextureDesc.Height = ImageHeight;
	TextureDesc.MipLevels = 0;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = RE.GetFormat();
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA SD = {};
	SD.pSysMem = &Image[0];
	SD.SysMemPitch = ImageWidth * sizeof(G3D::Color_ARGB);
	SD.SysMemSlicePitch = 0; // unused for anything other than a 3D texture

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = RE.GetFormat();
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture2D.MipLevels = 1;

	HR = RE.GetDevice()->CreateTexture2D(&TextureDesc, &SD, &pTexture);
	HR = RE.GetDevice()->CreateShaderResourceView(pTexture.Get(), &SRVDesc, &pTextureView);

	CreationComplete = true;
}

/*
Call this to perform all per-frame operations for this resource
Does nothing if Create has not completed successfully.

Inputs:
N/A

Outputs:
N/A

*/
void G3D::Texture2D::Draw()
{
	if (!CreationComplete) return;
	RE.GetImmediateContext()->PSSetShaderResources(0u, 1u, pTextureView.GetAddressOf());
}

/*
Call this to empty the buffers of their data. Call this after create, and before
draw. The object will need to be reinitialized if create needs to be called again.

Inputs:
N/A

Outputs:
N/A

*/
void G3D::Texture2D::Empty()
{
	Image.clear();
	Image.shrink_to_fit();
}