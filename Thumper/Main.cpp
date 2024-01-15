#include "WIN_WinWrapper.h"
#include "WIN_Window.h"
#include <string>

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	WIN::Window testWin;
	std::string allReceivedKeys = "";
	testWin.SetTitle(L"test 2");
	while (true) 
	{
		// process all messages pending, but to not block for new messages
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