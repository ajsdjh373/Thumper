/*
Author: Nathan Dunn
Module: WIN

See associated header file for more information
*/

#include "WIN_Window.h"
#include <windowsx.h>
#include <memory>
#include <stdexcept>

/*
Debug logger for windows messages. This will output to a debug output listener if one is present. Otherwise, it does nothing.
*/
#ifdef _DEBUG
#include <sstream>
static void WIN::DebugLogMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	std::wostringstream oss;
	switch (msg)
	{
	case WM_KEYDOWN:    oss << L"WM_KEYDOWN   wParam: " << wParam << L" lParam: " << lParam << L"\n"; break;
	case WM_KEYUP:      oss << L"WM_KEYUP     wParam: " << wParam << L" lParam: " << lParam << L"\n"; break;
	case WM_SYSKEYDOWN: oss << L"WM_SYSKEYDOWN wParam: " << wParam << L" lParam: " << lParam << L"\n"; break;
	case WM_SYSKEYUP:   oss << L"WM_SYSKEYUP  wParam: " << wParam << L" lParam: " << lParam << L"\n"; break;
	case WM_KILLFOCUS:  oss << L"WM_KILLFOCUS\n"; break;
	case WM_SETFOCUS:   oss << L"WM_SETFOCUS\n"; break;
	default: return; // return early to avoid calling OutputDebugString on untracked messages
	}
	OutputDebugStringW(oss.str().c_str());
}
#endif

/*
Constructor

Arguments:
N/A

Return:
N/A

Safeties and known issues:
- N/A

*/
WIN::Window::Window() :
	className(L"testclassname"),
	windowName(L"thumper test name"),
	height(320),
	width(640)
{
	// module handle
	// could use updating to GetModuleHandleEx
	hInst = GetModuleHandle(nullptr);

	// register the window class
	WNDCLASSEXW wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = className.c_str();
	wc.hIconSm = nullptr;
	if (RegisterClassExW(&wc) == 0)
	{
		throw std::runtime_error("RegisterClassExW failed");
	}

	// adjust actual window size so that width and height represent the usuable area not the total window size
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{
		throw std::runtime_error("AdjustWindowRect failed");
	}

	// create the window
	hWnd = CreateWindowExW(
		0,
		className.c_str(),
		windowName.c_str(),
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wr.right - wr.left,
		wr.bottom - wr.top,
		nullptr,
		nullptr,
		hInst,
		this
	);
	if (hWnd == NULL)
	{
		throw std::runtime_error("CreateWindowExW failed");
	}

	// show the window
	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

/*
Destructor

Arguments:
N/A

Return:
N/A

Safeties and known issues:
- N/A

*/
WIN::Window::~Window()
{
	// both of these functions can have errors, but IDK what to do with them so I'll ignore them
	DestroyWindow(hWnd);
	UnregisterClassW(className.c_str(), hInst);
}

/*
Sets the title of the window.

Arguments:
title		String for the title

Return:
N/A

Safeties and known issues:
- N/A

*/
void WIN::Window::SetTitle(const std::wstring& title)
{
	if (SetWindowTextW(hWnd, title.c_str()) == 0)
	{
		throw std::runtime_error("WIN::Window::SetTitle failed");
	}
}

/*
Loops through waiting windows messages.
If one of the waiting messages is a a quit message, it handles it. Otherwise, 
it dispatches the message to the implementor defined procedure HandleMessageSetup/Thunk.

Arguments:
N/A

Return:
Anything	Quit program message
Empty		Indicates nothing of interest, ignore return

Safeties and known issues:
- N/A

*/
std::optional<int> WIN::Window::ProcessMessageQue() noexcept
{
	MSG msg;

	// while queue has messages, remove and dispatch them (but do not block on empty queue)
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// check for quit because peekmessage does not signal this via return val
		if (msg.message == WM_QUIT)
		{
			// return optional wrapping int (arg to PostQuitMessage is in wparam) signals quit
			return (int)msg.wParam;
		}

		DispatchMessage(&msg);
	}

	// return empty optional when not quitting app
	return {};
}

/*
Get the handle to the window.

Arguments:
N/A

Return:
HWND		Unmodified handle for the window

Safeties and known issues:
- This is a copy. The window doesn't know what is currently using the handles, so it can't guarantee that the handle is still valid when the caller uses it.

*/
HWND WIN::Window::GetHandle() const
{
	return hWnd;
}

/*
Windows callback function for WNDCLASSEX.lpfnWndProc. This function forwards all message 
traffic to the default windows procedure until the window is created. Once the window is 
created, this function is replaced by HandleMsgThunk().

Arguments:
Defined by windows requirements for WNDCLASSEX.lpfnWndProc

Return:
Defined by windows requirements for WNDCLASSEX.lpfnWndProc

Safeties and known issues:
- N/A

*/
LRESULT CALLBACK WIN::Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
	if (msg == WM_NCCREATE)
	{
		// extract ptr to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// set WinAPI-managed user data to store ptr to window instance
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		// forward message to window instance handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	// if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

/*
Windows callback function for WNDCLASSEX.lpfnWndProc for after startup procedures. 
Forwards the message handling to a non static handler.

Arguments:
Defined by windows requirements for WNDCLASSEX.lpfnWndProc

Return:
Defined by windows requirements for WNDCLASSEX.lpfnWndProc

Safeties and known issues:
- N/A

*/
LRESULT CALLBACK WIN::Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// retrieve ptr to window instance
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward message to window instance handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

/*
Windows callback function for WNDCLASSEX.lpfnWndProc. This is the non-static version.

Arguments:
Defined by windows requirements for WNDCLASSEX.lpfnWndProc

Return:
Defined by windows requirements for WNDCLASSEX.lpfnWndProc

Safeties and known issues:
- N/A

*/
LRESULT WIN::Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// debug message viewing
#ifdef _DEBUG
	WIN::DebugLogMessage(msg, wParam, lParam);
#endif

	std::unique_ptr<MS::Button> Holder;
	int X;
	int Y;

	switch (msg)
	{
		// we don't want the DefProc to handle this message because
		// we want our destructor to destroy the window, so return 0 instead of break
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		// clear keystate when window loses focus to prevent input getting "stuck"
	case WM_KILLFOCUS:
		kbd.SetFocus(false);
		break;

		/*********** KEYBOARD MESSAGES ***********/
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		kbd.OnKeyDown(lParam, wParam);
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		kbd.OnKeyUp(lParam, wParam);
		break;
		/*********** END KEYBOARD MESSAGES ***********/

		/************* MOUSE MESSAGES ****************/
	case WM_MOUSEMOVE:
		// all movement tracked through MOUSEMOVE exclusively!
		X = GET_X_LPARAM(lParam);
		Y = GET_Y_LPARAM(lParam);
		
		if ((X > 0) && (X < width) && (Y > 0) && (Y < height))
		{
			// the cursor is inside the window
			ms.OnButtonDown(lParam, wParam, MS::Move);
			ms.SetFocus(true);
			kbd.SetFocus(true);
			if (!ms.CheckFocus())
			{
				SetCapture(hWnd);
			}
		}
		else
		{
			// the cursor is outside the window
			// do NOT allow inputs of any sort when the cursor is outside the window!
			ms.SetFocus(false);
			kbd.SetFocus(false);
			if (ms.CheckFocus())
			{
				ReleaseCapture();
			}
		}
		break;
	case WM_LBUTTONDOWN:
		ms.OnButtonDown(lParam, wParam, MS::Left);
		SetForegroundWindow(hWnd);
		break;
	case WM_RBUTTONDOWN:
		ms.OnButtonDown(lParam, wParam, MS::Right);
		SetForegroundWindow(hWnd);
		break;
	case WM_MBUTTONDOWN:
		ms.OnButtonDown(lParam, wParam, MS::Middle);
		SetForegroundWindow(hWnd);
		break;
	case WM_MOUSEWHEEL:
		ms.OnButtonDown(lParam, wParam, MS::Wheel);
		SetForegroundWindow(hWnd);
		break;

	case WM_LBUTTONUP:
		ms.OnButtonUp(lParam, MS::Left);
		break;
	case WM_RBUTTONUP:
		ms.OnButtonUp(lParam, MS::Right);
		break;
	case WM_MBUTTONUP:
		ms.OnButtonUp(lParam, MS::Middle);
		break;
		/************** END MOUSE MESSAGES **************/
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}