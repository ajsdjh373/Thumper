/*
Author: Nathan Dunn
Module: WIN

See associated header file for more information
*/
#include <fstream>
#include <filesystem>
#include "AST_Core.h"

/*
Checks if the filename contains characters that are invalid for windows.
*/
ERR::ErrorCodes AST::CheckFileNameForWindows(const std::string& fileName)
{
	if (fileName.find_first_of("\\/:*?\"<>|", 0) != std::string::npos)
	{
		return ERR::ErrorCodes::invalidWindowsFileName;
	}
	return ERR::ErrorCodes::okay;
}

/*
Checks if the filename contains characters that are invalid for windows, and that the extension is as specified in the arguments. Specify the extension without the period.
*/
ERR::ErrorCodes AST::CheckFileNameForWindows(const std::string& fileName, const std::string& extension)
{
	if (fileName.find_first_of("/:*?\"<>|", 0) != std::string::npos) // exlcuding / to allow for absolute paths
	{
		return ERR::ErrorCodes::invalidWindowsFileName;
	}
	if (fileName.size() < 2)
	{
		return ERR::ErrorCodes::invalidWindowsFileName;
	}
	std::string fileNameExtension = fileName.substr(fileName.size() - 2, 2);
	if (fileNameExtension.compare(extension) != 0)
	{
		return ERR::ErrorCodes::invalidFileExtension;
	}
	return ERR::ErrorCodes::okay;
}

/*
Default size for the vectors can be 0 because the expected workflow is to OpenCopy() before accessing anything. OpenCopy will QA the file.
*/
AST::G1::G1():
	vertexX {{}},
	vertexY {{}},
	vertexZ {{}},
	indices {{}},
	fileName {{}},
	id{ 0 }
{}

/*
Checks for a valid G1 file dataset. It requires:
-At least 3 vertices
-At least 3 vertices
-Equal XYZ vectors
-Indices in multiples of 3
-A valid windows filename
*/

ERR::ErrorCodes AST::G1::CheckIntegrity()
{
	if (vertexX.size() <= 3) 
	{
		return ERR::ErrorCodes::incompleteTriangleGeometryData;
	}
	if ((vertexX.size() != vertexY.size()) || (vertexX.size() != vertexZ.size())) 
	{ 
		return ERR::ErrorCodes::incompleteVectorData;
	}
	if ((indices.size() % 3) > 0) 
	{
		return ERR::ErrorCodes::incompleteVectorData;
	}
	if (indices.size() < 3)
	{
		return ERR::ErrorCodes::incompleteTriangleGeometryData;
	}
	return CheckFileNameForWindows(fileName);
}

/*
Opens a .G1 file and performs QA tests.
*/
ERR::ErrorCodes AST::G1::OpenCopy(std::string fileName)
{
	ERR::ErrorCodes currentCode = CheckFileNameForWindows(fileName, "G1");
	if (currentCode != ERR::ErrorCodes::okay)
	{
		ERR::errorTracker.PrintToLog("Error code " + std::to_string(currentCode) + " thrown while loading " + fileName + ", load operation aborted", __FILE__, __func__, __LINE__);
		return currentCode;
	}

	std::fstream fileStream;
	fileStream.open(fileName, std::ios::in | std::ios::binary | std::ios::ate);
	if (!fileStream.is_open())
	{
		currentCode = ERR::ErrorCodes::fileCannotBeOpened;
		ERR::errorTracker.PrintToLog("Error code " + std::to_string(currentCode) + " thrown while loading " + fileName + ", load operation aborted", __FILE__, __func__, __LINE__);
		return currentCode;
	}
	this->fileName = fileName;
	
	std::streamsize size = fileStream.tellg();
	fileStream.seekg(0, std::ios::beg);

	std::vector<char> fileBlob(size);
	if (!fileStream.read(fileBlob.data(), size))
	{
		currentCode = ERR::ErrorCodes::fileReadingError;
		ERR::errorTracker.PrintToLog("Error code " + std::to_string(currentCode) + " thrown while reading " + fileName + ", load operation aborted", __FILE__, __func__, __LINE__);
		return currentCode;
	}
	fileStream.close();

	// first 4 bytes are an int that should be 0x00000001. Read to verify endianness. It must be little endian because this is for windows x64, which is natively little endian.
	size_t requiredLength = sizeof(uint32_t);
	if (requiredLength > fileBlob.size())
	{
		currentCode = ERR::ErrorCodes::fileCorrupted;
		ERR::errorTracker.PrintToLog("Error code " + std::to_string(currentCode) + " thrown while reading " + fileName + ", means the file is corrupted, load operation aborted", __FILE__, __func__, __LINE__);
		return currentCode;
	}

	// holders used throughout
	uint32_t holder32;
	uint32_t holder32_2;

	memcpy(&holder32, &fileBlob[0], sizeof(uint32_t));
	if (holder32 != 0x00000001)
	{
		if (holder32 == 0x01000000)
		{
			currentCode = ERR::ErrorCodes::bigEndianData;
			ERR::errorTracker.PrintToLog("Error code " + std::to_string(currentCode) + " thrown while reading " + fileName + ", means the file is in big endian, load operation aborted", __FILE__, __func__, __LINE__);
			return currentCode;
		}
		currentCode = ERR::ErrorCodes::fileCorrupted;
		ERR::errorTracker.PrintToLog("Error code " + std::to_string(currentCode) + " thrown while reading " + fileName + ", means the file is corrupted, load operation aborted", __FILE__, __func__, __LINE__);
		return currentCode;
	}

	// next 4 are the ID of the asset as uint32
	size_t blockStart = 0;
	blockStart = requiredLength;
	requiredLength += sizeof(uint32_t);
	if (requiredLength > fileBlob.size())
	{
		currentCode = ERR::ErrorCodes::fileCorrupted;
		ERR::errorTracker.PrintToLog("Error code " + std::to_string(currentCode) + " thrown while reading " + fileName + ", means the file is corrupted, load operation aborted", __FILE__, __func__, __LINE__);
		return currentCode;
	}
	memcpy(&id, &fileBlob[blockStart], sizeof(uint32_t));

	// data now consists of blocks. Each block is preceeded by two uint32. The first identifies the purpose of the data in the block and its type. The second is the length of the data block in bytes. The blocks may be in any order.
	while (requiredLength < fileBlob.size())
	{
		blockStart = requiredLength;

		requiredLength += sizeof(uint32_t) * 2;
		if (requiredLength > fileBlob.size())
		{
			currentCode = ERR::ErrorCodes::fileCorrupted;
			ERR::errorTracker.PrintToLog("Error code " + std::to_string(currentCode) + " thrown while reading " + fileName + ", means the file is corrupted, load operation aborted", __FILE__, __func__, __LINE__);
			return currentCode;
		}

		memcpy(&holder32, &fileBlob[blockStart], sizeof(uint32_t));
		memcpy(&holder32_2, &fileBlob[blockStart + sizeof(uint32_t)], sizeof(uint32_t));

		if (holder32_2 == 0)
		{
			currentCode = ERR::ErrorCodes::fileCorrupted;
			ERR::errorTracker.PrintToLog("Error code " + std::to_string(currentCode) + " thrown while reading " + fileName + ", G1 data block of 0 length, load operation aborted", __FILE__, __func__, __LINE__);
			return currentCode;
		}

		switch (holder32)
		{
		default:
			currentCode = ERR::ErrorCodes::fileCorrupted;
			ERR::errorTracker.PrintToLog("Error code " + std::to_string(currentCode) + " thrown while reading " + fileName + ", G1 data block unrecognized, load operation aborted", __FILE__, __func__, __LINE__);
			return currentCode;
		case(0x00000001):
			// vertex X information in float32
			requiredLength += sizeof(uint8_t) * holder32_2;
			if (requiredLength > fileBlob.size())
			{
				currentCode = ERR::ErrorCodes::fileCorrupted;
				ERR::errorTracker.PrintToLog("Error code " + std::to_string(currentCode) + " thrown while reading " + fileName + ", G1 vertexX data block shorter than specified, load operation aborted", __FILE__, __func__, __LINE__);
				return currentCode;
			}
			if ((holder32_2 % sizeof(uint32_t)) > 0)
			{
				currentCode = ERR::ErrorCodes::fileCorrupted;
				ERR::errorTracker.PrintToLog("Error code " + std::to_string(currentCode) + " thrown while reading " + fileName + ", G1 data block vertexX ID 0x00000001 length % sizeof() > 0, load operation aborted", __FILE__, __func__, __LINE__);
				return currentCode;
			}
			vertexX.resize(holder32_2 / sizeof(uint32_t));
			memcpy(&vertexX[0], &fileBlob[blockStart + sizeof(uint32_t) * 2], holder32_2);
			break;
		case(0x00000002):
			// vertex Y information in float32
			requiredLength += sizeof(uint8_t) * holder32_2;
			if (requiredLength > fileBlob.size())
			{
				currentCode = ERR::ErrorCodes::fileCorrupted;
				ERR::errorTracker.PrintToLog("Error code " + std::to_string(currentCode) + " thrown while reading " + fileName + ", G1 vertexY data block shorter than specified, load operation aborted", __FILE__, __func__, __LINE__);
				return currentCode;
			}
			if ((holder32_2 % sizeof(uint32_t)) > 0)
			{
				currentCode = ERR::ErrorCodes::fileCorrupted;
				ERR::errorTracker.PrintToLog("Error code " + std::to_string(currentCode) + " thrown while reading " + fileName + ", G1 data block vertexY ID 0x00000002 length % sizeof() > 0, load operation aborted", __FILE__, __func__, __LINE__);
				return currentCode;
			}
			vertexY.resize(holder32_2 / sizeof(uint32_t));
			memcpy(&vertexY[0], &fileBlob[blockStart + sizeof(uint32_t) * 2], holder32_2);
			break;
		case(0x00000003):
			// vertex Z information in float32
			requiredLength += sizeof(uint8_t) * holder32_2;
			if (requiredLength > fileBlob.size())
			{
				currentCode = ERR::ErrorCodes::fileCorrupted;
				ERR::errorTracker.PrintToLog("Error code " + std::to_string(currentCode) + " thrown while reading " + fileName + ", G1 vertexZ data block shorter than specified, load operation aborted", __FILE__, __func__, __LINE__);
				return currentCode;
			}
			if ((holder32_2 % sizeof(uint32_t)) > 0)
			{
				currentCode = ERR::ErrorCodes::fileCorrupted;
				ERR::errorTracker.PrintToLog("Error code " + std::to_string(currentCode) + " thrown while reading " + fileName + ", G1 data block vertexZ ID 0x00000003 length % sizeof() > 0, load operation aborted", __FILE__, __func__, __LINE__);
				return currentCode;
			}
			vertexZ.resize(holder32_2 / sizeof(uint32_t));
			memcpy(&vertexZ[0], &fileBlob[blockStart + sizeof(uint32_t) * 2], holder32_2);
			break;
		case(0x00000004):
			// index information in uint16
			requiredLength += sizeof(uint8_t) * holder32_2;
			if (requiredLength > fileBlob.size())
			{
				currentCode = ERR::ErrorCodes::fileCorrupted;
				ERR::errorTracker.PrintToLog("Error code " + std::to_string(currentCode) + " thrown while reading " + fileName + ", G1 index data block shorter than specified, load operation aborted", __FILE__, __func__, __LINE__);
				return currentCode;
			}
			if ((holder32_2 % sizeof(uint16_t)) > 0)
			{
				currentCode = ERR::ErrorCodes::fileCorrupted;
				ERR::errorTracker.PrintToLog("Error code " + std::to_string(currentCode) + " thrown while reading " + fileName + ", G1 data block index ID 0x00000004 length % sizeof() > 0, load operation aborted", __FILE__, __func__, __LINE__);
				return currentCode;
			}
			indices.resize(holder32_2 / sizeof(uint16_t));
			memcpy(&indices[0], &fileBlob[blockStart + sizeof(uint32_t) * 2], holder32_2);
			break;
		}

	}

	return CheckIntegrity();

}