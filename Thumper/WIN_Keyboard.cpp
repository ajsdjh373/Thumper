/*
Author: Nathan Dunn

See matching header file
*/

#include "WIN_Keyboard.h"

/*
constructor for the keyboard
*/
WIN::Keyboard::Keyboard() :
	LastRaw{ -1 },
	LastASCII{ -1 },
	OffsetL{ KBDASCII::a - KBDRAW::A },
	OffsetU{ KBDASCII::A - KBDRAW::A },
	OffsetSN{ KBDASCII::Zero - KBDRAW::Std0 },
	OffsetSNS{ KBDASCII::Exclamation - KBDRAW::Std0 },
	OffsetNN{ KBDASCII::Zero - KBDRAW::Num0 },
	OffsetS{ KBDASCII::BackTick - KBDRAW::BackTick },
	OffsetSS{ KBDASCII::Tilde - KBDRAW::BackTick },
	Focus{ true },
	BytesInShort{ sizeof(short int) },
	BytesInInt{ sizeof(int) },
	BytesInLong{ sizeof(long int) },
	BytesInLongLong{ sizeof(long long int) }
{}

/*
Use this to reset the key state arrays to no keys down
*/
void WIN::Keyboard::ClearState()
{
	for (int i = 0; i < StatesRaw.size(); i++)
	{
		StatesRaw[i].Down = false;
		StatesRaw[i].Total = 0;
	}

	for (int i = 0; i < StatesASCII.size(); i++)
	{
		StatesASCII[i].Down = false;
		StatesASCII[i].Total = 0;
	}

	LastRaw = -1;
	LastASCII = -1;
}

/*
Use this to reset the state for ASCII characters only
*/
void WIN::Keyboard::ClearASCII()
{
	for (int i = 0; i < StatesASCII.size(); i++)
	{
		StatesASCII[i].Down = false;
		StatesASCII[i].Total = 0;
	}

	LastASCII = -1;
}

/*
Use this to reset the state of a raw key's total
*/
void WIN::Keyboard::ClearRawTotal(int RawKey)
{
	if (!((RawKey >= StatesRaw.size()) || (RawKey < 0)))
	{
		StatesRaw[RawKey].Total = 0;
	}
}

/*
Use this to reset the state of an ASCII key's total
*/
void WIN::Keyboard::ClearASCIITotal(int ASCIIKey)
{
	if (!((ASCIIKey >= StatesASCII.size()) || (ASCIIKey < 0)))
	{
		StatesRaw[ASCIIKey].Total = 0;
	}
}

/*
Use this to parse the contents of WM_KEYDOWN and WM_SYSKEYDOWN messages
Will do nothing is Focus is false
*/
void WIN::Keyboard::OnKeyDown(int lParam, int wParam)
{
	if (Focus)
	{
		// check that wParam isn't out of bounds
		int Index = 0;
		if ((wParam >= RawHashTable.size()) || (wParam < 0))
		{
			// bad, so we'll set it to NotKey since nothing should EVER use that
			Index = KBDRAW::NotKey;
		}
		else
		{
			// good to proceed
			Index = RawHashTable[wParam]; // casting 32 bit to 64 bit
		}

		const unsigned int Mask = 16777216u; // binary 00000001 00000000 00000000 00000000
		int ExtendedKey = 0;

		// need to check lParam for left and normal shift, tab, and alt
		if ((Index == KBDRAW::Shift) || (Index == KBDRAW::Alt) || (Index == KBDRAW::Ctrl))
		{
			//To extract from B into A, apply a mask M where you want the bits copied: A=(B&M)|(A&~M)
			ExtendedKey = lParam & Mask;
		}
		if (ExtendedKey == Mask)
		{
			// the masked bit was high, so this is an extended key
			switch (Index)
			{
			default:
				Index = KBDRAW::NotKey;
				break;

			case KBDRAW::Shift:
				Index = KBDRAW::ShiftR;
				break;

			case KBDRAW::Alt:
				Index = KBDRAW::AltR;
				break;

			case KBDRAW::Ctrl:
				Index = KBDRAW::CtrlR;
				break;
			}
		}

		StatesRaw[Index].Down = true;
		StatesRaw[Index].Total++;
		if (Index > KBDRAW::Excluded)
		{
			LastRaw = Index;
		}

		Index = ASCIIHashTable[Index](this, Index);
		StatesASCII[Index].Down = true;
		StatesASCII[Index].Total++;
		if (Index > KBDASCII::Excluded)
		{
			LastASCII = Index;
		}
	}
}

/*
Use this to parse the contents of WM_KEYUP and WM_SYSKEYUP messages
Will do nothing if Focus is false
*/
void WIN::Keyboard::OnKeyUp(int lParam, int wParam)
{
	if (Focus)
	{
		// check that wParam isn't out of bounds
		int Index = 0;
		if ((wParam >= RawHashTable.size()) || (wParam < 0))
		{
			// bad, so we'll set it to NotKey since nothing should EVER use that
			Index = KBDRAW::NotKey;
		}
		else
		{
			// good to proceed
			Index = RawHashTable[wParam]; // casting 32 bit to 64 bit
		}

		const int Mask = 16777216; // binary 00000001 00000000 00000000 00000000
		int ExtendedKey = 0;

		// need to check lParam for left and normal shift, tab, and alt
		if ((Index == KBDRAW::Shift) || (Index == KBDRAW::Alt) || (Index == KBDRAW::Ctrl))
		{
			//To extract from B into A, apply a mask M where you want the bits copied: A=(B&M)|(A&~M)
			ExtendedKey = lParam & Mask;
		}
		if (ExtendedKey == Mask)
		{
			// the masked bit was high, so this is an extended key
			switch (Index)
			{
			default:
				Index = KBDRAW::NotKey;
				break;

			case KBDRAW::Shift:
				Index = KBDRAW::ShiftR;
				break;

			case KBDRAW::Alt:
				Index = KBDRAW::AltR;
				break;

			case KBDRAW::Ctrl:
				Index = KBDRAW::CtrlR;
				break;
			}
		}

		StatesRaw[Index].Down = false;
		StatesRaw[Index].Total = 0;

		Index = ASCIIHashTable[Index](this, Index);
		StatesASCII[Index].Down = false;
		StatesASCII[Index].Total = 0;
	}
}

/*
Use to set the focus state of the keyboard
Setting the focus to false will clear the key states and block input
*/
void WIN::Keyboard::SetFocus(bool Input)
{
	if (Input)
	{
		// focus on the keyboard
		Focus = true;
	}
	else
	{
		// focus off the keyboard
		Focus = false;
		ClearState();
	}
}

/*
Use this to access a raw key
*/
std::unique_ptr<KBDRAW::Key> WIN::Keyboard::CheckRaw(int RawKey)
{
	if ((RawKey >= StatesRaw.size()) || (RawKey < 0))
	{
		return std::make_unique<KBDRAW::Key>(StatesRaw[KBDRAW::Excluded]);
	}
	return std::make_unique<KBDRAW::Key>(StatesRaw[RawKey]);
}

/*
Use this to access an ASCII key
*/
std::unique_ptr<KBDASCII::Key> WIN::Keyboard::CheckASCII(int ASCIIKey)
{
	if ((ASCIIKey >= StatesRaw.size()) || (ASCIIKey < 0))
	{
		return std::make_unique<KBDASCII::Key>(StatesASCII[KBDASCII::Excluded]);
	}
	return std::make_unique<KBDASCII::Key>(StatesASCII[ASCIIKey]);
}

/*
Use this to qwery if the focus is on the keyboard
*/
bool WIN::Keyboard::CheckFocus()
{
	return Focus;
}

/*
Use this to get the index of the last raw key that was pushed
*/
int WIN::Keyboard::CheckLastRaw()
{
	return LastRaw;
}

/*
Use this to get the index of the last ASCII key that was pushed
*/
int WIN::Keyboard::CheckLastASCII()
{
	return LastASCII;
}

/*
Give this a raw key, and it will give you the index of the corresponding ascii key
Produces: no ASCII character, use for things which shouldn't produce text
*/
int WIN::Keyboard::HashBlank(int Key)
{
	// inputs are unused, we only have it because of convenience for the hash table
	return KBDASCII::Excluded;
}

/*
Give this a raw key, and it will give you the index of the corresponding ascii key
Produces: entire alphabet, lower and uppercase
*/
int WIN::Keyboard::HashASCIILetter(int Key)
{
	if (CheckRaw(KBDRAW::Shift)->Down || CheckRaw(KBDRAW::ShiftR)->Down)
	{
		return Key + OffsetU;
	}

	return Key + OffsetL;
}

/*
Give this a raw key, and it will give you the index of the corresponding ascii key
Produces: 0 through 9 from standard number keys, !@#$%^&*()
*/
int WIN::Keyboard::HashASCIIStdNum(int Key)
{
	if (CheckRaw(KBDRAW::Shift)->Down || CheckRaw(KBDRAW::ShiftR)->Down)
	{
		return Key + OffsetSNS;
	}

	return Key + OffsetSN;
}

/*
Give this a raw key, and it will give you the index of the corresponding ascii key
Produces: 0 through 9, / * - + . from numpad input only
*/
int WIN::Keyboard::HashASCIINumpad(int Key)
{
	if ((Key >= KBDRAW::Num0) && (Key <= KBDRAW::Num9))
	{
		return Key + OffsetNN;
	}

	switch (Key)
	{
	default:
		return KBDASCII::Excluded;
		break;

	case KBDRAW::Divide:
		return KBDASCII::FrontSlash;
		break;

	case KBDRAW::Multiply:
		return KBDASCII::Asterisk;
		break;

	case KBDRAW::Subtract:
		return KBDASCII::Dash;
		break;

	case KBDRAW::Plus:
		return KBDASCII::Plus;
		break;

	case KBDRAW::Separator:
		return KBDASCII::Enter;
		break;

	case KBDRAW::Dot:
		return KBDASCII::Period;
		break;
	}
}

/*
Give this a raw key, and it will give you the index of the corresponding ascii key
Produces: `~-_=+[{]}\|;:'",<.>/?
*/
int WIN::Keyboard::HashASCIISymbol(int Key)
{
	if (CheckRaw(KBDRAW::Shift)->Down || CheckRaw(KBDRAW::ShiftR)->Down)
	{
		return Key + OffsetSS;
	}

	return Key + OffsetS;
}

/*
Give this a raw key, and it will give you the index of the corresponding ascii key
Produces: enter, backspace, space, delete, tab
*/
int WIN::Keyboard::HashASCIIControl(int Key)
{
	switch (Key)
	{
	default:
		return KBDASCII::Excluded;
		break;

	case KBDRAW::Enter:
		return KBDASCII::Enter;
		break;

	case KBDRAW::Del:
		return KBDASCII::Delete;
		break;

	case KBDRAW::Backspace:
		return KBDASCII::Backspace;
		break;

	case KBDRAW::Space:
		return KBDASCII::Space;
		break;

	case KBDRAW::Tab:
		return KBDASCII::Tab;
		break;
	}
}
