/*
Author: Nathan Dunn
Module: G3D

See associated header file for more information
*/

#include "G3D_Camera.h"
#include <math.h>

/*
Default constructor
*/
G3D::Camera::Camera() :
	Focus{ 0, 0, 0 },
	Radius{ 30 },
	Elevation{ 70 },
	Azimuth{ 0 },
	CacheFocus{ 0, 0, 0 },
	CacheRadius{ 30 },
	CacheElevation{ 70 },
	CacheAzimuth{ 0 }
{}

/*
Constructor
*/
G3D::Camera::Camera(float PointAtX, float PointAtY, float PointAtZ, float Radius, float Elevation, float Azimuth, float FoV, float NearPlane, float FarPlane) :
	Focus{ PointAtX, PointAtY, PointAtZ },
	Radius{ Radius },
	Elevation{ Elevation },
	Azimuth{ Azimuth },
	FoV{ FoV },
	FarPlane{ FarPlane },
	NearPlane{ NearPlane },
	CacheFocus{ PointAtX, PointAtY, PointAtZ },
	CacheRadius{ Radius },
	CacheElevation{ Elevation },
	CacheAzimuth{ Azimuth }
{}

/*
Caches all current camera orientation information
*/
void G3D::Camera::Cache()
{
	CacheFocus = Focus;
	CacheRadius = Radius;
	CacheElevation = Elevation;
	CacheAzimuth = Azimuth;
}

/*
Reverts the camera orientation information to the last cash
*/
void G3D::Camera::Revert()
{
	Focus = CacheFocus;
	Radius = CacheRadius;
	Elevation = CacheElevation;
	Azimuth = CacheAzimuth;
}

/*
Returns the left handed matrix for the camera's orientation.
*/
DirectX::XMMATRIX G3D::Camera::GetMatrix() const noexcept
{
	/*
	Roll, pitch, and yaw controls have been removed because they
	are not needed for OSRS style camera control and introduce extra
	complexity to world space click detection.

	Elevation and Azimuth are implemented correctly, despite appearances!
	Elevation is from the camera's frame of reference, so + elevation is looking up, 0 elevation is a horizontal camera
	Azimuth is rotating around +z with a + azimuth moving from +x to +y
	*/
	float NearPlaneSize;

	NearPlaneSize = 2.0f * NearPlane * tan(0.5f * FoV * 3.1415f / 180.0f);

	/*
	const auto CameraPosition = DirectX::XMVector3Transform(
		DirectX::XMVectorSet(Focus.X, Focus.Y, Radius + Focus.Z, 0.0f),
		DirectX::XMMatrixRotationRollPitchYaw(0.0f, -(Elevation + 90) * 3.1415f / 180.0f, 0.0f) * DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, -Azimuth * 3.1415f / 180.0f)
	);

	return DirectX::XMMatrixLookAtLH(
		CameraPosition,
		DirectX::XMVectorSet(Focus.X, Focus.Y, Focus.Z, 0.0f),
		DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)) *
		DirectX::XMMatrixScaling(360.0f / 640.0f, 1.0f, 1.0f) *
		DirectX::XMMatrixPerspectiveLH(NearPlaneSize, NearPlaneSize, NearPlane, FarPlane);
	*/

	//const auto CameraPosition = DirectX::XMVectorAdd(
	//	DirectX::XMVector3Transform(
	//		DirectX::XMVectorSet(0, 0, Radius + Focus.Z, 0.0f),
	//		DirectX::XMMatrixRotationRollPitchYaw(0.0f, Elevation * 3.1415f / 180.0f, 0.0f) *
	//		DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, Azimuth * 3.1415f / 180.0f)
	//	), 
	//	DirectX::XMVectorSet(Focus.X, Focus.Y, 0.0f, 0.0f)
	//);
	//
	//return DirectX::XMMatrixLookAtLH(
	//	CameraPosition,
	//	DirectX::XMVectorSet(Focus.X, Focus.Y, Focus.Z, 0.0f),
	//	DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)) *
	//	DirectX::XMMatrixScaling(360.0f / 640.0f, 1.0f, 1.0f) *
	//	DirectX::XMMatrixPerspectiveLH(NearPlaneSize, NearPlaneSize, NearPlane, FarPlane);

	const auto CameraPosition = DirectX::XMVectorAdd(
		DirectX::XMVector3Transform(
			DirectX::XMVectorSet(0, 0, Radius, 0.0f),
			DirectX::XMMatrixRotationRollPitchYaw(0.0f, Elevation * 3.1415f / 180.0f, 0.0f) *
			DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, Azimuth * 3.1415f / 180.0f)
		),
		DirectX::XMVectorSet(Focus.X, Focus.Y, Focus.Z, 0.0f)
	);

	return DirectX::XMMatrixLookAtLH(
		CameraPosition,
		DirectX::XMVectorSet(Focus.X, Focus.Y, Focus.Z, 0.0f),
		DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)) *
		DirectX::XMMatrixScaling(360.0f / 640.0f, 1.0f, 1.0f) *
		DirectX::XMMatrixPerspectiveLH(NearPlaneSize, NearPlaneSize, NearPlane, FarPlane);
}

/*
Function for generating a vector and a point that represents a line passing
through the camera and a point in screen space

Accepts the X and Y screen space coordinates and the point and vector to be
modified with the return values

Returns true when the inputs aren't in the correct format, returns false
when they're okay

Note: need to switch over to TMath once TMath is verified to be safe
*/
//bool GFX_3D::Camera::CastRay(int X, int Y, Window& Wnd, std::vector<float>& Point, std::vector<float>& Vector)
//{
//	/*
//	Step 0, safety
//	Due to some querk, I cannot take a reference to a c style array, so I am
//	using vector and require that it have a size of 3. This won't be necessary
//	once I switch to TMath types
//	*/
//	if (Point.size() != 3)
//	{
//		return true;
//	}
//	if (Vector.size() != 3)
//	{
//		return true;
//	}
//
//	/*
//	Step 1, calculate the position. Everything from here
//	on out is using the default directx coordinate system.
//	*/
//	// Camera location, manual version from CameraPosition in GetMatrix()
//	Vector = {
//		-Radius * sin(Elevation * 3.1415f / 180) * sin((Azimuth - 90) * 3.1415f / 180),
//		Radius * sin(Elevation * 3.1415f / 180) * cos((Azimuth - 90) * 3.1415f / 180),
//		Radius * cos(Elevation * 3.1415f / 180)
//	};
//
//	/*
//	Step 2, store camera position as the point in the return
//	*/
//	Point[0] = Vector[0] + Focus.X;
//	Point[1] = Vector[1] + Focus.Y;
//	Point[2] = Vector[2] + Focus.Z;
//
//	/*
//	Step 3, camera view vector
//	*/
//	Vector[0] = -Vector[0];
//	Vector[1] = -Vector[1];
//	Vector[2] = -Vector[2];
//
//	/*
//	Step 4, change magnitude of camera view vector to the distance to the near plane
//	*/
//	float Factor = 0;;
//	Factor = NearPlane / sqrt(Vector[0] * Vector[0] + Vector[1] * Vector[1] + Vector[2] * Vector[2]);
//	Vector[0] = Vector[0] * Factor;
//	Vector[1] = Vector[1] * Factor;
//	Vector[2] = Vector[2] * Factor;
//
//	/*
//	Step 5, calculate the screenspace displacement vector
//	*/
//	float NearPlaneSize = 0;
//	NearPlaneSize = 2.0f * NearPlane * tan(0.5f * FoV * 3.1415f / 180.0f);
//
//	// displacement magnitudes in the screen space (note 2D)
//	// accepts with the screenspace origin in top left of screen, outputs with origin in center
//	float Transform[2] = { 0, 0 };
//	Transform[0] = ((double)X / Wnd.width - 0.5) * NearPlaneSize * Wnd.width / Wnd.height;
//	Transform[1] = ((double)Y / Wnd.height - 0.5) * NearPlaneSize;
//
//	/*
//	Step 6, calculate unit vectors in world space that align with the screenspace x and y
//
//	Note: on all cross products, I also multiply by -1 because this is a left handed
//	coordinate system but the formulas are for a right handed system
//	*/
//	// cross camera view with up direction <0, 0 , 1>, then normalize
//	float TransformX[3] = { 0, 0, 0 };
//	TransformX[0] = -Vector[1];
//	TransformX[1] = Vector[0];
//	TransformX[2] = 0;
//	Factor = sqrt(TransformX[0] * TransformX[0] + TransformX[1] * TransformX[1] + TransformX[2] * TransformX[2]);
//	TransformX[0] = TransformX[0] / Factor;
//	TransformX[1] = TransformX[1] / Factor;
//	TransformX[2] = TransformX[2] / Factor;
//	// cross view with TransformX to get TransformY, then normalize
//	float TransformY[3] = { 0, 0, 0 };
//	//TransformY[0] = Vector[1] * TransformX[2] - Vector[2] * TransformX[1];
//	//TransformY[1] = Vector[2] * TransformX[0] - Vector[0] * TransformX[2];
//	//TransformY[2] = Vector[0] * TransformX[1] - Vector[1] * TransformX[0];
//	TransformY[0] = Vector[2] * TransformX[1] - Vector[1] * TransformX[2];
//	TransformY[1] = Vector[0] * TransformX[2] - Vector[2] * TransformX[0];
//	TransformY[2] = Vector[1] * TransformX[0] - Vector[0] * TransformX[1];
//	Factor = sqrt(TransformY[0] * TransformY[0] + TransformY[1] * TransformY[1] + TransformY[2] * TransformY[2]);
//	TransformY[0] = TransformY[0] / Factor;
//	TransformY[1] = TransformY[1] / Factor;
//	TransformY[2] = TransformY[2] / Factor;
//
//	/*
//	Step 7, apply the transform magnitude to the transform directions
//	*/
//	TransformX[0] = TransformX[0] * Transform[0];
//	TransformX[1] = TransformX[1] * Transform[0];
//	TransformX[2] = TransformX[2] * Transform[0];
//	TransformY[0] = TransformY[0] * Transform[1];
//	TransformY[1] = TransformY[1] * Transform[1];
//	TransformY[2] = TransformY[2] * Transform[1];
//
//	/*
//	Step 8, add the transform vectors to the view vector, resulting in the click vector
//	*/
//	Vector[0] = Vector[0] + TransformX[0] + TransformY[0];
//	Vector[1] = Vector[1] + TransformX[1] + TransformY[1];
//	Vector[2] = Vector[2] + TransformX[2] + TransformY[2];
//
//	/*
//	Step 9, normalize the click vector
//	*/
//	Factor = sqrt(Vector[0] * Vector[0] + Vector[1] * Vector[1] + Vector[2] * Vector[2]);
//	Vector[0] = Vector[0] / Factor;
//	Vector[1] = Vector[1] / Factor;
//	Vector[2] = Vector[2] / Factor;
//
//	// we're done here
//	return false;
//}