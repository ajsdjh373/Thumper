/*
Author: Nathan Dunn
Module: G3D

Multipurpose utilitarian data structures.

Known bugs and limitations:
- N/A

*/

#pragma once

namespace UTL
{
	struct attitude
	{
		float x = 0;
		float y = 0;
		float z = 0;
		float roll = 0;
		float pitch = 0;
		float yaw = 0;
		float xScale = 1;
		float yScale = 1;
		float zScale = 1;
	};

	struct point
	{
		float x = 0;
		float y = 0;
		float z = 0;
	};
}