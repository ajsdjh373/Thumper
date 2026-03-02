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
//#include "Window.h"

namespace G3D
{
	class Camera; // forward declare so everything doesn't have to be tabbed
}

class G3D::Camera
{
public:
	// public member functions
	Camera();
	Camera(float PointAtX, float PointAtY, float PointAtZ, float Radius, float Elevation, float Azimuth, float FoV, float NearPlane, float FarPlane);
	void Cache();
	void Revert();
	DirectX::XMMATRIX GetMatrix() const noexcept;
	//bool CastRay(int X, int Y, Window& Wnd, std::vector<float>& Point, std::vector<float>& Vector);

public:
	// public member variables
	struct Point
	{
		float X;
		float Y;
		float Z;
	};
	Point Focus;
	float Radius;
	float Elevation;
	float Azimuth;
	float FoV;
	float NearPlane;
	float FarPlane;

private:
	// private member variables
	Point CacheFocus;
	float CacheRadius;
	float CacheElevation;
	float CacheAzimuth;
};