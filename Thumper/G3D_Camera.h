/*
Author: Nathan Dunn
Module: G3D

Camera class for the 3D camera in the G3D module. Designed to emulate the OSRS camera behavior, so it is not a true 6 DoF generic camera.

Known bugs and limitations:
- need to be edited to fit with the G3D conventions
*/

#pragma once
#include <DirectXMath.h>
#include <vector>
#include "UTL_Utilities.h"
#include "G3D_RenderEngine.h"
//#include "Window.h"

namespace G3D
{
	class Camera; // forward declare so everything doesn't have to be tabbed
}

class G3D::Camera
{
public:
	// public member functions
	Camera() = delete;
	Camera(UTL::attitude attitude, float nearPlane, float farPlane, float fov);
	void Update(UTL::attitude attitude);
	void Update(UTL::attitude attitude, float nearPlane, float farPlane, float fov);
	DirectX::XMMATRIX GetMatrix(G3D::RenderEngine& re) const noexcept;
	//bool CastRay(int X, int Y, Window& Wnd, std::vector<float>& Point, std::vector<float>& Vector);

private:
	// private member functions

public:
	// public member variables
	UTL::attitude attitude;
	float nearPlane;
	float farPlane;
	float fov;

private:
	// private member variables
	
};