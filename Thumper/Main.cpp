#include "WIN_WinWrapper.h"
#include <string>
#include <chrono>
#include <thread>
#include "WIN_Window.h"
#include "G3D_Include.h"
#include "ERR_ErrorEngine.h"

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	try
	{
		ERR::errorTracker.PrintToLog("window init successful");

		// temporary clock stuff
		int inputPeriod_ms = 25;
		long long lastTime_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
		long long nextTime_ms = 0;

		// window
		WIN::Window testWin;
		std::string allReceivedKeys = "";
		testWin.SetTitle(L"test 2");

		// G3D testing
		// renderer
		// texture
		// shader
		// tile obj
		// camera, set renderer to use camera

		// main loop
		while (true)
		{
			// loop timing code
			/* I tried doing a solution using:
			
			nextTime_ms = lastTime_ms + inputPeriod_ms;
			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() < nextTime_ms)
			{
				std::this_thread::sleep_until(std::chrono::steady_clock::time_point(std::chrono::milliseconds(nextTime_ms)));
			}
			lastTime_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

			But that turned out to be quite imprecise. For a superior timer solution, I should probably sleep for most of the period and spin the while loop for the last 5-10 ms.
			Getting this timing to be precise is an important part of getting different threads to work at consistent rates.

			Look into timeBeginPeriod() and SetThreadPriority() functions from windows.h to improve the reliability of timing resolution across all machines.
			
			*/
			nextTime_ms = lastTime_ms + inputPeriod_ms;
			while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() < nextTime_ms) {}
			lastTime_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
			
			// process all messages pending, but not block new messages
			if (const auto ecode = WIN::Window::ProcessMessageQue())
			{
				// if return optional has value, means we're quitting so return exit code
				//return *ecode;
				return 0;
			}

			bool GDown = testWin.kbd.CheckRaw(KBDRAW::SemiColon)->Down;
			bool ColonDown = testWin.kbd.CheckASCII(KBDASCII::VerticalBar)->Down;
			if (GDown || ColonDown)
			{
				testWin.SetTitle(L"good");
			}
			else
			{
				testWin.SetTitle(L"nah");
			}
			
			

		};

		return 0;

	}
	catch (const std::exception& e)
	{
		// otherwise unhandled exception from the rest of the program, constitutes a critical error for the program
		MessageBoxA(NULL, e.what(), "Error", MB_ICONERROR);
		return 1;
	}
	
	return 1;
}