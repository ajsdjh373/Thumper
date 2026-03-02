/*
Author: Nathan Dunn
Module: G3D

This is the core class for the G3D module. All D3D11 API calls are managed through
this, effectively making this module a low level wrapper for the D3D11 API.

In regards to memory management and object handling:
The G3D module manages all objects. You don't create objects directly.
Call ObjInit() to create an object. This includes the creation of D3D11 resources and by extension involves memory allocation.
Call ObjDraw() to toggle the object being drawn on screen.
Call ObjKill() to remove the object from memory entirely (also stopping it from being drawn if it was being drawn).
Call ObjUpdateAttitude() to provide a new set of 9 DoF attitude data to the object (x, y, z, roll, pitch, yaw, x scale, y scale, z scale).
Note that there is no function to change the asset data (textures, geometry, etc). This is because the asset data is meant to be static. If you want to change the asset data, create a new object with the new asset data and kill the old one.

Asset data is NOT managed, cached, or in any other way stored in the G3D module other than in the relevant D3D11 resources. G3D does not store references to it - it copies when needed, and that's all. Maintaining this isolation barrier between the G3D functionality and the asset loading system allows for G3D to do whatever it needs to with the asset data without risking concurrency issues (once G3D is moved into its own thread). It also makes ObjInit functions costly due to the necessary memory copying.

Known bugs and limitations:
- designed for 32 bit operation, all data structures are 32 bit
- not all header files which need windows.h include it, instead including this file

*/

#pragma once
#include "WIN_WinWrapper.h" // if I get windows lib problems, try switching back to windows.h
//#include <Windows.h> // many things depend on this include
#include <d3d11.h>
#include <wrl.h>
#include <vector>

namespace G3D
{
	// avoiding tabs
	class RenderEngine;

	// avoiding include circular includes
	//class Obj_WireFrame;
	//class Shader_WireFrame;
}

class G3D::RenderEngine
{
public:
	// public member functions
	//RenderEngine();
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