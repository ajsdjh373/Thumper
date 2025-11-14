/*
Author: Nathan Dunn
Module: ERR

This is an in-engine error handling and reporting mechanism. The purpose is to support debugging and logging. 
Unlike most modules, no objects need to be declared because this has one global object. Just include the include file.
Member functions of this module return an int type. Identify result codes via the ERR::ErrorCodes enum.

The log file remains open for the duration of the global object, which is to say the duration of the program.

Known bugs and limitations:
- N/A
*/

#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <mutex>

namespace ERR
{
	class ErrorEngine;
	enum ErrorCodes;
}

enum ERR::ErrorCodes
{
	okay = 0,
	actionFailed = 1,
	hrFailed = 2,
};

class ERR::ErrorEngine
{
	public:
		// public member types

	private:
		// private member types

	public:
		// public member functions
		// constructors first, then destructors, then everything else
		ErrorEngine();
		~ErrorEngine();
		ERR::ErrorCodes PrintToLog(std::string output);
		ERR::ErrorCodes TestHR(HRESULT HR);
		ERR::ErrorCodes TestHR(HRESULT HR, std::string outputOnError);

	private:
		// private member functions
		std::string MakeTimestamp(); // utility to generate date time in text

	public:
		// public member variables

	private:
		// private member variables
		std::string logFileName;
		std::ofstream logFile;
		std::mutex logFileLock;

};

namespace ERR
{
	// global error tracking - definition in cpp file
	extern ErrorEngine errorTracker;
}