/*
Author: Nathan Dunn
Module: G3D

A general collection of types and functions for the module, focused on math

Known bugs and limitations:
- N/A
*/

#pragma once

namespace G3D
{
	/*
	Operable for vector math
	*/
	struct Point_Cartesian
	{
		float X;
		float Y;
		float Z;
	};

	struct Point_Cylindrical
	{
		float Radius;
		float Elevation;
		float Azimuth;
	};

	struct Orientation
	{
		float Roll;
		float Pitch;
		float Yaw;
	};

	/*
	Non-operating for D3D11 input descriptions
	*/
	struct Vertex_P // Point
	{
		float X;
		float Y;
		float Z;
	};

	struct Vertex_PT // Point, Texture UV
	{
		float X;
		float Y;
		float Z;
		float U;
		float V;
	};

	struct Vertex_PTN // Point, Texture UV, Normals
	{
		float X;
		float Y;
		float Z;
		float U;
		float V;
		float NX;
		float NY;
		float NZ;
	};

	struct Color_ARGB
	{
		char Alpha;
		char Red;
		char Green;
		char Blue;
	};

}

// this replaces TMPRVertex
// I need to convert Camera to this
// I need to do overload operators