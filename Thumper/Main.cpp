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
		WIN::Window mainWin;
		std::string allReceivedKeys = "";
		mainWin.SetTitle(L"test 2");

		// G3D testing
		G3D::RenderEngine renderEngine(mainWin.GetHandle(), mainWin.width, mainWin.height);
		// add a wire frame object
		std::vector<UTL::point> vertixXYZ =
		{
			{0.0f, 0.0f, 0.0f},
			{0.5f, 0.0f, 0.0f},
			{0.5f, 0.5f, 0.0f}
		};
		std::vector<unsigned short> indexData =
		{
			0, 1, 2
		};
		G3D::Obj_WireFrame wireFrameObject{ vertixXYZ ,indexData, renderEngine };
		G3D::Shader_WireFrame wireFrameShader{ renderEngine };

		// camera
		UTL::globalFrame cameraGlobalFrame{ 0.0f, 0.0f, -5.0f };
		UTL::bodyFrame cameraBodyFrame{0.0f, 0.0f, 0.0f, 1, 1, 1 };

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

			bool GDown = mainWin.kbd.CheckRaw(KBDRAW::SemiColon)->Down;
			bool ColonDown = mainWin.kbd.CheckASCII(KBDASCII::VerticalBar)->Down;
			if (GDown || ColonDown)
			{
				mainWin.SetTitle(L"good");
			}
			else
			{
				mainWin.SetTitle(L"nah");
			}

			// camera keyboard controls
			if (mainWin.kbd.CheckASCII(KBDASCII::w)->Down)
			{
				cameraGlobalFrame.x += 0.01;
			}
			if (mainWin.kbd.CheckASCII(KBDASCII::s)->Down)
			{
				cameraGlobalFrame.x -= 0.01;
			}
			if (mainWin.kbd.CheckASCII(KBDASCII::a)->Down)
			{
				cameraGlobalFrame.y += 0.01;
			}
			if (mainWin.kbd.CheckASCII(KBDASCII::d)->Down)
			{
				cameraGlobalFrame.y -= 0.01;
			}
			if (mainWin.kbd.CheckASCII(KBDASCII::r)->Down)
			{
				cameraGlobalFrame.z += 0.01;
			}
			if (mainWin.kbd.CheckASCII(KBDASCII::f)->Down)
			{
				cameraGlobalFrame.z -= 0.01;
			}
			if (mainWin.kbd.CheckASCII(KBDASCII::W)->Down)
			{
				cameraBodyFrame.pitch -= 0.01;
			}
			if (mainWin.kbd.CheckASCII(KBDASCII::S)->Down)
			{
				cameraBodyFrame.pitch += 0.01;
			}
			if (mainWin.kbd.CheckASCII(KBDASCII::A)->Down)
			{
				cameraBodyFrame.roll -= 0.01;
			}
			if (mainWin.kbd.CheckASCII(KBDASCII::D)->Down)
			{
				cameraBodyFrame.roll += 0.01;
			}
			if (mainWin.kbd.CheckASCII(KBDASCII::R)->Down)
			{
				cameraBodyFrame.yaw -= 0.01;
			}
			if (mainWin.kbd.CheckASCII(KBDASCII::F)->Down)
			{
				cameraBodyFrame.yaw += 0.01;
			}
			
			// G3D testing
			renderEngine.camera.Update(cameraBodyFrame, cameraGlobalFrame);
			wireFrameShader.Draw(renderEngine);
			wireFrameObject.Draw(renderEngine);
			renderEngine.PresentFrame();

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