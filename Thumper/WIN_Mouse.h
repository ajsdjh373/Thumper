/*
Author: Nathan Dunn

This is the revamped mouse handling class.
Functions from this class are called by a window's lpfnWndProc function,
ie the message handler for the window.

This version is reduces the layers of abstraction required to set and
access the mouse state. Chili's mouse implementation was a lot better
than the keyboard, but for the sake of consistency I rewrote this as
well to use a similar interface.

This does not support M4 and M5 buttons. Only scroll wheel direction
is recorded due to peculiarities in wParam behavior that do match the
documentation of WM_MOUSEWHEEL.

For discussions on implementation decisions, consider checking Keyboard.
Notes there may answer your questions because this was moddled after Keyboard.

Note: the scroll wheel is unique when compared to all other keyboard and
mouse inputs. Unlike keys and buttons, the wheel lacks an equivalent to
KEYUP. There is no built in way to determine the lifetime of the input.
Rather than determining that here, it is up to the user of the mouse
class to determine how that happens. Mouse will keep only the latest
scroll input. To aide this interface, ClearWheel() is provided. Use it
to reset the only the wheel's value.

*/

#pragma once
#include <array>
#include <functional>
#include <memory>

namespace WIN
{
	class Mouse;
}

/*
Alias namespace for mouse input
Naming exception: due to the use and nature of this, it is allowed to be global
*/
namespace MS
{
	enum MS
	{
		NotButton,
		Move,
		Left,
		Right,
		Middle,
		Wheel,
		EnumLength
	};

	struct Button
	{
		bool Down = false;
		unsigned int Total = 0;
		int X = 0;
		int Y = 0;
		int Aux = 0;
	};

}

/*
Main mouse class
*/
class WIN::Mouse
{
public:
	// public member functions
	Mouse();

	void ClearState();
	void ClearTotal(int Button);
	void OnButtonDown(int lParam, int wParam, int Button);
	void OnButtonUp(int lParam, int Button);
	void SetFocus(bool Input);

	std::unique_ptr<MS::Button> CheckButton(int Button);
	bool CheckFocus();
	void ClearWheel();

private:
	// private member functions

public:
	// public member variables

private:
	// private member variables
	std::array<MS::Button, MS::EnumLength> StatesMouse;
	bool Focus;

};