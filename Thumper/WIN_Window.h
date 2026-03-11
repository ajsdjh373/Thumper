/*
Author: Nathan Dunn
Module: WIN

Wrapper for creating the window within the windows API, and all that is handled through that such as keyboard and mouse inputs.

Known bugs and limitations:
- N/A
*/

#pragma once
#include "WIN_WinWrapper.h"
#include "WIN_Keyboard.h"
#include "WIN_Mouse.h"
#include <string>
#include <optional>

namespace WIN
{
	class Window;
	void DebugLogMessage(UINT msg, WPARAM wParam, LPARAM lParam);
}

class WIN::Window
{
public:
	// public member types

private:
	// private member types

public:
	// public member functions
	Window();
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const std::wstring& title);
	static std::optional<int> ProcessMessageQue() noexcept;
	HWND GetHandle() const;

private:
	// private member functions
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

public:
	// public member variables
	WIN::Keyboard kbd;
	WIN::Mouse ms;
	unsigned short height;
	unsigned short width;

private:
	// private member variables
	const std::wstring className;
	std::wstring windowName;
	HINSTANCE hInst;
	HWND hWnd;

};
