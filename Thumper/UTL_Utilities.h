/*
Author: Nathan Dunn
Module: UTL

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
		float roll = 0; // rotation around x axis
		float pitch = 0; // rotation around y axis
		float yaw = 0; // rotation around z axis
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