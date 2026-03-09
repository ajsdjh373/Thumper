/*
Author: Nathan Dunn
Module: G3D

See associated header file for more information
*/

#include "G3D_Camera.h"
#include <cmath>

/*
No surprises.

Arguments:
attitude		9 DoF attitude. Scale represents zoom and aspect ratio distortion
nearPlane		distance to the near plane
farPlane		distance to the far plane

Return:
N/A

Safeties and known issues:
- N/A

*/
G3D::Camera::Camera(UTL::vector3f attitude, UTL::vector3f position, float nearPlane, float farPlane, float fov) :
	attitude{ attitude },
	position{ position },
	nearPlane{ nearPlane },
	farPlane{ farPlane },
	fov{ fov }
{}

/*
Immediately updates the camera attitude.

Arguments:
attitude		new attitude

Return:
N/A

Safeties and known issues:
- N/A

*/
void G3D::Camera::Update(UTL::vector3f attitude, UTL::vector3f position)
{
	this->attitude = attitude;
	this->position = position;
}

/*
Immediately updates the camera attitude.

Arguments:
attitude		new attitude

Return:
N/A

Safeties and known issues:
- N/A

*/
void G3D::Camera::Update(UTL::vector3f attitude, UTL::vector3f position, float nearPlane, float farPlane, float fov)
{
	this->attitude = attitude;
	this->position = position;
	this->farPlane = farPlane;
	this->nearPlane = nearPlane;
	this->fov = fov;
}

/*
Calculate and return the transform matrix associated with the camera.

Arguments:
N/A

Return:
Matrix transform for an object due to the camera's position and orientation

Safeties and known issues:
- N/A

*/
//UTL::matrix4x4f G3D::Camera::GetMatrix(unsigned short widthInPixels, unsigned short heightInPixels) const noexcept
//{
//	//UTL::vector4f q_base = UTL::QuaternionFromEuler(attitude);
//	//UTL::vector4f q_rotation{ 0.70710677f, 0, 0.70710677f, 0 };
//	//UTL::vector4f q = UTL::QuaternionMultiply(q_rotation, q_base);
//	//UTL::vector3f newAttitude = { attitude.r1c1, attitude.r2c1 - UTL::pi / 2, attitude.r3c1 };
//	//UTL::vector4f q = UTL::QuaternionFromEuler(newAttitude);
//	//UTL::vector4f q = UTL::QuaternionFromEuler(attitude);
//	//
//	//// inverse quaternion, negate XYZ
//	//q.r2c1 = -q.r2c1;
//	//q.r3c1 = -q.r3c1;
//	//q.r4c1 = -q.r4c1;
//	//
//	//UTL::matrix4x4f rotation = UTL::RotationFromQuaternion(q);
//	//UTL::matrix4x4f basis =
//	//{
//	//	0, 0, -1, 0,
//	//	0, 1, 0, 0,
//	//	1, 0, 0, 0,
//	//	0, 0, 0, 1
//	//};
//	//UTL::matrix4x4f view = UTL::Multiply(rotation, basis);
//	//
//	//UTL::vector3f invertedPosition = { -position.r1c1, -position.r2c1, -position.r3c1 };
//	//UTL::matrix4x4f translation = UTL::TranslationFromVector(invertedPosition);
//	//
//	//UTL::matrix4x4f transform = UTL::Multiply(view, translation);
//	//
//	//float aspectRatio = (float)widthInPixels / (float)heightInPixels;
//	//UTL::matrix4x4f projection =
//	//{
//	//	1 / (aspectRatio * std::tan(fov / 2)), 0, 0, 0,
//	//	0, 1 / (std::tan(fov / 2)), 0, 0,
//	//	0, 0, farPlane / (farPlane - nearPlane), -farPlane * nearPlane / (farPlane - nearPlane),
//	//	0, 0, 1, 0
//	//};
//	//transform = UTL::Multiply(projection, transform);
//	//
//	//return transform;
//	return { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
//}


/*
Function for generating a vector and a point that represents a line passing
through the camera and a point in screen space

Accepts the X and Y screen space coordinates and the and vector to be
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