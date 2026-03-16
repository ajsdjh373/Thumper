/*
Author: Nathan Dunn
Module: AST

Asset (AST) library. This library transforms between system files and asset data structures. The convention is that the library does not permanently own asset data, it only moves, transforms, etc. If there are precompute or validation steps, that is allowed.

Known bugs and limitations:
- N/A
*/

#include <vector>
#include <string>
#include "ERR_ErrorEngine.h"

#pragma once

namespace AST
{
	ERR::ErrorCodes CheckFileNameForWindows(const std::string& fileName);
	ERR::ErrorCodes CheckFileNameForWindows(const std::string& fileName, const std::string& extension);

	class G1 // XYZ vertex data, 16 bit indices, id
	{
	public:
		// public member functions
		G1();
		ERR::ErrorCodes CheckIntegrity();
		ERR::ErrorCodes OpenCopy(std::string fileName);
		ERR::ErrorCodes SaveAs(std::string fileName);
		ERR::ErrorCodes Save();

	private:
		// private member functions

	public:
		// public member variables
		std::vector<float> vertexX;
		std::vector<float> vertexY;
		std::vector<float> vertexZ;
		std::vector<uint16_t> indices;
		std::string fileName;
		uint32_t id;

	private:
		// private member variables
	};
}
