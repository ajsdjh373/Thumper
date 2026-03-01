/*
Author: Nathan Dunn
Module: G3D

This is the core class for the G3D module. All D3D11 API calls are managed through
this, effectively making this module a low level wrapper for the D3D11 API.

In regards to memory management:
In general, the G3D module manages the interconnection of resources and what is
done with the resources, but it does not manage the resource itself. It is the
responsibility of the implementor to store resources. This is because the
management of memory is beyond the scope of graphics, and will change depending on
what the graphics module is being used to visualize. The notable exception to this
is vertex and index buffers. Vertex and index buffers are tied to the drawn object,
ie the map of buffers to objects on screen is 1:1. Even if the buffer is shared, it
is still 1:1 buffer to unique object (in this instance the buffer would also be
managed inside the Obj, not externally). Compare this to a shader, where 1 shader
can map to any number of different objects. When a Vertex or Index buffer can be
removed, an Empty function will be supplied to do so.

Known bugs and limitations:
- designed for 32 bit operation, all data structures are 32 bit
- not all header files which need windows.h include it, instead including this file

*/

#pragma once
#include "WIN_WinWrapper.h" // if I get windows lib problems, try switching back to windows.h
//#include <Windows.h> // many things depend on this include
#include <d3d11.h>
#include <wrl.h>
#include "G3D_Camera.h"

namespace G3D
{
	class RenderEngine; // forward declare so everything doesn't have to be tabbed
}

class G3D::RenderEngine
{
public:
	// public member functions
	RenderEngine();
	RenderEngine(HWND hWnd, const unsigned short Width, const unsigned short Height);
	void PresentFrame();

	// access functions
	int GetWidth();
	int GetHeight();
	Microsoft::WRL::ComPtr<ID3D11Device> GetDevice();
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetImmediateContext();
	DXGI_FORMAT GetFormat();

private:
	// private member functions

public:
	// public member variables
	G3D::Camera Cam;

	// return holder for D3D11 API calls
	// replace this when error handling is implemented
	HRESULT HR;

private:
	// private member variables
	unsigned short int Width;
	unsigned short int Height;
	D3D_FEATURE_LEVEL DeviceFeatureLevel;
	DXGI_FORMAT TextureFormat;

	// D3D11 objects
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pImmediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;

};