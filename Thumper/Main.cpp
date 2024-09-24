#include "WIN_WinWrapper.h"
#include "WIN_Window.h"
#include <string>
#include <chrono>
#include <thread>

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	try
	{
		// temporary clock stuff
		int inputPeriod_ms = 25;
		long long lastTime_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
		long long nextTime_ms = 0;

		WIN::Window testWin;
		std::string allReceivedKeys = "";
		testWin.SetTitle(L"test 2");
		while (true)
		{
			nextTime_ms = lastTime_ms + inputPeriod_ms;
			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() < nextTime_ms)
			{
				std::this_thread::sleep_until(std::chrono::steady_clock::time_point(std::chrono::milliseconds(nextTime_ms)));
			}
			lastTime_ms = nextTime_ms;

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

			
			//throw std::runtime_error("Test error: forced exception"); // test for error handling message box

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