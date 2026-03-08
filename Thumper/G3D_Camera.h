/*
Author: Nathan Dunn
Module: G3D

Camera class for the 3D camera in the G3D module. Designed to be a 6 DoF generic camera. The camera is designed to point in the +x direction with 0 RPY.

Known bugs and limitations:
- need to be edited to fit with the G3D conventions
*/

#pragma once
#include <DirectXMath.h>
#include <vector>
#include "UTL_Utilities.h"
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
	Camera(UTL::vector3f attitude, UTL::vector3f position, float nearPlane, float farPlane, float fov);
	void Update(UTL::vector3f attitude, UTL::vector3f position);
	void Update(UTL::vector3f attitude, UTL::vector3f position, float nearPlane, float farPlane, float fov);
	UTL::matrix4x4f GetMatrix(unsigned short widthInPixels, unsigned short heightInPixels) const noexcept;
	//bool CastRay(int X, int Y, Window& Wnd, std::vector<float>& Point, std::vector<float>& Vector);

private:
	// private member functions

public:
	// public member variables
	UTL::vector3f attitude;
	UTL::vector3f position;
	float nearPlane;
	float farPlane;
	float fov;

private:
	// private member variables
	
};