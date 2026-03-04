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
	struct globalFrame
	{
		float x = 0; // global frame of reference
		float y = 0; // global frame of reference
		float z = 0; // global frame of reference
	};

	struct bodyCenteredAttitude
	{
		float roll = 0; // rotation around x axis in body frame of reference, 0 aligns with global
		float pitch = 0; // rotation around y axis in body frame of reference
		float yaw = 0; // rotation around z axis in body frame of reference
		float xScale = 1; // scale in body frame of reference
		float yScale = 1; // scale in body frame of reference
		float zScale = 1; // scale in body frame of reference
	};

	struct point
	{
		float x = 0;
		float y = 0;
		float z = 0;
	};
}