/*
Author: Nathan Dunn
Module: ERR

See associated header file for more information
*/

#include "WIN_WinWrapper.h"
#include <fstream>
#include <chrono>
#include <sstream>
#include "ERR_ErrorEngine.h"

/*
Constructor for the ErrorEngine class

Arguments:
N/A

Return:
N/A

Safeties and known issues:
- N/A

*/
ERR::ErrorEngine::ErrorEngine():
	logFileName{"log.txt"}
{
	logFile = std::ofstream(logFileName, std::ios::trunc);

    // if a log file can't be made now, then we're screwed so just notify the user
    if (!logFile.is_open())
    {
        MessageBoxA(NULL, "Failed to create log file", "Error", MB_ICONERROR);
        return;
    }

    // write a basic entry
    logFile << MakeTimestamp() << "Log begin" << std::endl;

}

/*
Destructor for the ErrorEngine class.

Arguments:
N/A

Return:
N/A

Safeties and known issues:
- N/A

*/
ERR::ErrorEngine::~ErrorEngine()
{
    // this is the primary way to close the log file - exiting the program
    if (logFile.is_open()) { logFile.close(); }

}

/*
Immediately attempts to write a string to the log file.

Arguments:
N/A

Return:
Error code
ERR::ErrorCodes::okay           if successful
ERR::ErrorCodes::actionFailed   file streaming error

Safeties and known issues:
- I have not checked to see if the chrono call is safe

*/
ERR::ErrorCodes ERR::ErrorEngine::PrintToLog(std::string output)
{
    std::scoped_lock(logFileLock);
    try
    {
        logFile << MakeTimestamp() << output << std::endl;
        return ERR::ErrorCodes::okay;
    }
    catch (...)
    {
        return ERR::ErrorCodes::actionFailed;
    }
    
}

/*
Processes a windows HRESULT and returns an error code based on the result. If HR fails, it logs the error and HR.

Arguments:
- HR		any windows HRESULT

Return:
ERR::ErrorCodes::okay           if successful
ERR::ErrorCodes::hrFailed       hr didn't pass windows SUCCEEDED check (is negative)

Safeties and known issues:
- n/a

*/
ERR::ErrorCodes TestHR(HRESULT HR)
{
    try
    {
        if (SUCCEEDED(HR))
        {
            return ERR::ErrorCodes::okay;
        }
        else
        {
            std::stringstream ss;
            ss << "0x" << std::hex << std::uppercase << HR;
            PrintToLog("HR failed: " + ss.str());
            return ERR::ErrorCodes::hrFailed;
        }
    }
    catch (...)
    {
        return ERR::ErrorCodes::actionFailed;
    }

};

/*
Processes a windows HRESULT and returns an error code based on the result. If HR fails, it logs the error and HR, followed by the caller's specified output.

Arguments:
- HR		        any windows HRESULT
- outputOnError	    optional string to append to the log file if the hr is not successful, defaults to empty string if not provided

Return:
ERR::ErrorCodes::okay           if successful
ERR::ErrorCodes::hrFailed       hr didn't pass windows SUCCEEDED check (is negative)

Safeties and known issues:
- n/a

*/
ERR::ErrorCodes TestHR(HRESULT HR, std::string outputOnError)
{
try
    {
        if (SUCCEEDED(HR))
        {
            return ERR::ErrorCodes::okay;
        }
        else
        {
            std::stringstream ss;
            ss << "0x" << std::hex << std::uppercase << HR;
            PrintToLog("HR failed: " + ss.str() + " " + outputOnError);
            return ERR::ErrorCodes::hrFailed;
        }
    }
    catch (...)
    {
        return ERR::ErrorCodes::actionFailed;
    }
};

/*
Generates a milliseconds timestamp that should be put before every entry in the log.

Arguments:
N/A

Return:
Timestamp with a tab at the end.

Safeties and known issues:
- I have no checked to see if the chrono call is safe

*/
std::string ERR::ErrorEngine::MakeTimestamp()
{
    long long timeNow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    return std::to_string(timeNow) + "\t";
}

namespace ERR
{
    // global error tracking
    ErrorEngine errorTracker;
}
