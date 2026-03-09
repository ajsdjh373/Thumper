/*
Author: Nathan Dunn
Module: G3D

See associated header file for more information
*/

#include <dxgi1_2.h>
#include "G3D_RenderEngine.h"
#include "ERR_ErrorEngine.h"

/*
Constructor
Do NOT use this constructor directly.
The only reason the function exists is so that it can be owned as a member variable of other classes. In such a scenario, reassign the RenderEngine object to a properly constructed object in the constructor of the owner.

Inputs:
N/A

Outputs:
N/A

*/
/*
G3D::RenderEngine::RenderEngine()
{

}
*/

/*
Always use this constructor!

Inputs:
hWnd		handle for the window that this will render to
Width		width of the output window in pixels
Height		height of the output window in pixels

Outputs:
N/A

*/
G3D::RenderEngine::RenderEngine(HWND hWnd, const unsigned short Width, const unsigned short Height) :
	Height{ Height },
	Width{ Width },
	camera{ { 0, 0, 0 }, {0, 0, 0}, 0.1f, 1000.0f, UTL::pi/3 }
{
	// create device and swap chain
	UINT DeviceFlags = 0u;
#ifndef NDEBUG
	DeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	DeviceFeatureLevel = D3D_FEATURE_LEVEL_11_0; // 11_1 only supported with win8 and newer
	TextureFormat = DXGI_FORMAT_B8G8R8A8_UNORM;

	DXGI_SWAP_CHAIN_DESC SwapDesc = {};
	SwapDesc.BufferDesc.Width = 0; // width and height of 0 fetches the size from the output window hWnd
	SwapDesc.BufferDesc.Height = 0;
	SwapDesc.BufferDesc.RefreshRate.Numerator = 0;
	SwapDesc.BufferDesc.RefreshRate.Denominator = 1u;
	SwapDesc.BufferDesc.Format = TextureFormat;
	SwapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapDesc.SampleDesc.Count = 1;
	SwapDesc.SampleDesc.Quality = 0;
	SwapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapDesc.BufferCount = 1;
	SwapDesc.OutputWindow = hWnd;
	SwapDesc.Windowed = TRUE;
	SwapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	SwapDesc.Flags = 0;

	ERR::errorTracker.TestHR(
		D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			DeviceFlags,
			&DeviceFeatureLevel,
			1u,
			D3D11_SDK_VERSION,
			&SwapDesc,
			&pSwapChain,
			&pDevice,
			NULL,
			&pImmediateContext
		)
		, __FILE__, __func__, __LINE__
	);

	// gain access to texture subresource from the swap chain, then create a render target for it
	// passing a description to CreateRenderTargetView is required if I want to enable mipmaps
	Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;

	ERR::errorTracker.TestHR(pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer), __FILE__, __func__, __LINE__);
	ERR::errorTracker.TestHR(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pRenderTarget), __FILE__, __func__, __LINE__);

	// create the depth stencil state and set the context to it
	D3D11_DEPTH_STENCIL_DESC DSDesc = {};
	DSDesc.DepthEnable = TRUE;
	DSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DSDesc.DepthFunc = D3D11_COMPARISON_LESS;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState; // this pointer to the state is local
	ERR::errorTracker.TestHR(pDevice->CreateDepthStencilState(&DSDesc, &pDSState), __FILE__, __func__, __LINE__); // this creates the state, which is destroyed when the Device is destroyed
	pImmediateContext->OMSetDepthStencilState(pDSState.Get(), 1u);

	// create the texture for the depth stencil to store depth information in
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC DepthDesc = {};
	DepthDesc.Width = (UINT)Width;
	DepthDesc.Height = (UINT)Height;
	DepthDesc.MipLevels = 1u;
	DepthDesc.ArraySize = 1u;
	DepthDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DepthDesc.SampleDesc.Count = 1u;
	DepthDesc.SampleDesc.Quality = 0u;
	DepthDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	ERR::errorTracker.TestHR(pDevice->CreateTexture2D(&DepthDesc, nullptr, &pDepthStencil), __FILE__, __func__, __LINE__);

	// create the depth stencil's texture (where it stored the data for depths)
	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc = {};
	DSVDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSVDesc.Texture2D.MipSlice = 0u;
	DSVDesc.Flags = 0u; // read only specification
	HR = ERR::errorTracker.TestHR(pDevice->CreateDepthStencilView(pDepthStencil.Get(), &DSVDesc, &pDepthStencilView), __FILE__, __func__, __LINE__);

	// set the output merger to use the swap chain texture and the depth stencil texture as the output textures
	pImmediateContext->OMSetRenderTargets(1u, pRenderTarget.GetAddressOf(), pDepthStencilView.Get());

	/*
	Viewports

	I know the viewports can be used to tell what portion of the render target should be rendered in the rasterizer stage, but I have no idea what they do beyond that.

	You can set a very large number of viewports, and the geometry shader can select which one gets used. Why? What is this feature meant for? I have read speculation that it could be used for mirrors and split screen stuff, but the only example I have found that actually used viewports is:
	https://stackoverflow.com/questions/64831393/how-to-achieve-split-screen-effect-in-directx11

	*/
	D3D11_VIEWPORT VP;
	VP.Width = (FLOAT)Width;
	VP.Height = (FLOAT)Height;
	VP.MinDepth = 0.0f;
	VP.MaxDepth = 1.0f;
	VP.TopLeftX = 0.0f;
	VP.TopLeftY = 0.0f;
	pImmediateContext->RSSetViewports(1u, &VP);

	// the default rasterizer state is to draw wire frames - intended to only be used with Shader_WireFrame. Switch to D3D11_FILL_SOLID for everything else.
	D3D11_RASTERIZER_DESC RastDesc = {};
	RastDesc.FillMode = D3D11_FILL_WIREFRAME;
	RastDesc.CullMode = D3D11_CULL_NONE; // should change
	RastDesc.FrontCounterClockwise = FALSE;
	RastDesc.DepthClipEnable = TRUE;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRastState;
	ERR::errorTracker.TestHR(GetDevice()->CreateRasterizerState(&RastDesc, &pRastState), __FILE__, __func__, __LINE__);
	pImmediateContext->RSSetState(pRastState.Get()); // call RSSetState again with a different desc to affect subsequent draw calls

}

/*
Call to change the active buffer and reset the buffer that is available for writing.
Call this after all draw functions are called.

Inputs:
N/A

Outputs:
N/A

*/
void G3D::RenderEngine::PresentFrame()
{
	HR = ERR::errorTracker.TestHR(pSwapChain->Present(0u, 0u), __FILE__, __func__, __LINE__);
	// when error handling gets implemented, call pDevice->GetDeviceRemovedReason() like chili did

	// clear the render targets
	// replace this once a struct is in place for representing color
	const float Color[] = { 0.53f, 0.81f, 0.92f, 1.0f };
	pImmediateContext->ClearRenderTargetView(pRenderTarget.Get(), Color);
	pImmediateContext->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

/*
Access (get) function for window width

Inputs:
N/A

Outputs:
Window width in pixels

*/
int G3D::RenderEngine::GetWidth()
{
	return Width;
}

/*
Access (get) function for window height

Inputs:
N/A

Outputs:
Window height in pixels

*/
int G3D::RenderEngine::GetHeight()
{
	return Height;
}

/*
Access (get) function for the D3D device

Inputs:
N/A

Outputs:
windows com pointer for D3D device

*/
Microsoft::WRL::ComPtr<ID3D11Device> G3D::RenderEngine::GetDevice()
{
	return pDevice;
}

/*
Access (get) function for the D3D data format being used by the device

Inputs:
N/A

Outputs:
copy of the D3D data format

*/
DXGI_FORMAT G3D::RenderEngine::GetFormat()
{
	return TextureFormat;
}

/*
Access (get) function for the D3D device context

Inputs:
N/A

Outputs:
copy of the D3D device context

*/
Microsoft::WRL::ComPtr<ID3D11DeviceContext> G3D::RenderEngine::GetImmediateContext()
{
	return pImmediateContext;
}