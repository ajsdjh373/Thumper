/*
Author: Nathan Dunn

See matching header file
*/

#include "WIN_Mouse.h"

/*
Constructor
*/
WIN::Mouse::Mouse() :
	Focus{ true }
{}

/*
Use this to reset all the buttons and wheel
*/
void WIN::Mouse::ClearState()
{
	for (int i = 0; i < StatesMouse.size(); i++)
	{
		StatesMouse[i].Down = false;
		StatesMouse[i].Total = 0;
		StatesMouse[i].X = 0;
		StatesMouse[i].Y = 0;
		StatesMouse[i].Aux = 0;
	}
}

/*
Use this to reset the total for a given button
*/
void WIN::Mouse::ClearTotal(int Button)
{
	if (!((Button >= StatesMouse.size()) || (Button < 0)))
	{
		StatesMouse[Button].Total = 0;
	}
}

/*
Use this to set the mouse state in conjunction with any BUTTONDOWN message, MOUSEMOVE, or MOUSEWHEEL
*/
void WIN::Mouse::OnButtonDown(int lParam, int wParam, int Button)
{
	if (Focus && !((Button >= StatesMouse.size()) || (Button < 0)))
	{
		// bit masks
		unsigned int LowOrder = 65535u;
		unsigned int HighOrder = 4294901760u;
		unsigned int Sign = 2147483648u;

		// extract the low order word from lParam as the x coordinate
		LowOrder = LowOrder & lParam;

		// extract the high order word from lParam as the y coordinate
		HighOrder = HighOrder & lParam;
		HighOrder = HighOrder >> 16;

		// set values
		if ((Button < StatesMouse.size()) || (Button >= 0))
		{
			StatesMouse[Button].X = LowOrder; // casting unsigned to signed
			StatesMouse[Button].Y = HighOrder; // casting unsigned to signed
			StatesMouse[Button].Total++;
			StatesMouse[Button].Down = true;

			if (Button == MS::Wheel)
			{
				// extract the higher order word from wParam while preserving sign
				Sign = Sign & wParam | 1;
				StatesMouse[Button].Aux = Sign; // casting unsigned int to signed int
			}
		}
	}
}

/*
Use this with BUTTONUP messages only
*/
void WIN::Mouse::OnButtonUp(int lParam, int Button)
{
	if (Focus && !((Button >= StatesMouse.size()) || (Button < 0)))
	{
		// extract the low order word from lParam as the x coordinate
		unsigned int LowOrder = 65535u;
		LowOrder = LowOrder & lParam;

		// extract the high order word from lParam as the y coordinate
		unsigned int HighOrder = 4294901760u;
		HighOrder = HighOrder & lParam;
		HighOrder = HighOrder >> 16;

		// set values
		if ((Button < StatesMouse.size()) || (Button >= 0))
		{
			StatesMouse[Button].X = LowOrder; // casting unsigned to signed
			StatesMouse[Button].Y = HighOrder; // casting unsigned to signed
			StatesMouse[Button].Total = 0;
			StatesMouse[Button].Down = false;
		}
	}
}

/*
Use this to set the mouse focus
When focus is false, inputs will be blocked and states cleared
*/
void WIN::Mouse::SetFocus(bool Input)
{
	if (Input)
	{
		// focus on the mouse
		Focus = true;
	}
	else
	{
		// focus off the mouse
		Focus = false;
		ClearState();
	}
}

/*
Use this to access the current state of a specified button
*/
std::unique_ptr<MS::Button> WIN::Mouse::CheckButton(int Button)
{
	if ((Button >= StatesMouse.size()) || (Button < 0))
	{
		return std::make_unique<MS::Button>(StatesMouse[MS::NotButton]);
	}
	return std::make_unique<MS::Button>(StatesMouse[Button]);
}

/*
Use this to access the current state of the Focus
*/
bool WIN::Mouse::CheckFocus()
{
	return Focus;
}

/*
Use this to zero the scroll wheel's state
*/
void WIN::Mouse::ClearWheel()
{
	StatesMouse[MS::Wheel].Aux = 0;
}
