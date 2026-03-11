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

		if ((Index == KBDRAW::Shift) || (Index == KBDRAW::ShiftR))
		{
			// shift needs to immediately change the state of shift affected controls. Not the case for typing, but certainly for games.
			if (StatesASCII[KBDASCII::a].Down) { StatesASCII[KBDASCII::a].Down = false; StatesASCII[KBDASCII::A].Down = true; }
			if (StatesASCII[KBDASCII::b].Down) { StatesASCII[KBDASCII::b].Down = false; StatesASCII[KBDASCII::B].Down = true; }
			if (StatesASCII[KBDASCII::c].Down) { StatesASCII[KBDASCII::c].Down = false; StatesASCII[KBDASCII::C].Down = true; }
			if (StatesASCII[KBDASCII::d].Down) { StatesASCII[KBDASCII::d].Down = false; StatesASCII[KBDASCII::D].Down = true; }
			if (StatesASCII[KBDASCII::e].Down) { StatesASCII[KBDASCII::e].Down = false; StatesASCII[KBDASCII::E].Down = true; }
			if (StatesASCII[KBDASCII::f].Down) { StatesASCII[KBDASCII::f].Down = false; StatesASCII[KBDASCII::F].Down = true; }
			if (StatesASCII[KBDASCII::g].Down) { StatesASCII[KBDASCII::g].Down = false; StatesASCII[KBDASCII::G].Down = true; }
			if (StatesASCII[KBDASCII::h].Down) { StatesASCII[KBDASCII::h].Down = false; StatesASCII[KBDASCII::H].Down = true; }
			if (StatesASCII[KBDASCII::i].Down) { StatesASCII[KBDASCII::i].Down = false; StatesASCII[KBDASCII::I].Down = true; }
			if (StatesASCII[KBDASCII::j].Down) { StatesASCII[KBDASCII::j].Down = false; StatesASCII[KBDASCII::J].Down = true; }
			if (StatesASCII[KBDASCII::k].Down) { StatesASCII[KBDASCII::k].Down = false; StatesASCII[KBDASCII::K].Down = true; }
			if (StatesASCII[KBDASCII::l].Down) { StatesASCII[KBDASCII::l].Down = false; StatesASCII[KBDASCII::L].Down = true; }
			if (StatesASCII[KBDASCII::m].Down) { StatesASCII[KBDASCII::m].Down = false; StatesASCII[KBDASCII::M].Down = true; }
			if (StatesASCII[KBDASCII::n].Down) { StatesASCII[KBDASCII::n].Down = false; StatesASCII[KBDASCII::N].Down = true; }
			if (StatesASCII[KBDASCII::o].Down) { StatesASCII[KBDASCII::o].Down = false; StatesASCII[KBDASCII::O].Down = true; }
			if (StatesASCII[KBDASCII::p].Down) { StatesASCII[KBDASCII::p].Down = false; StatesASCII[KBDASCII::P].Down = true; }
			if (StatesASCII[KBDASCII::q].Down) { StatesASCII[KBDASCII::q].Down = false; StatesASCII[KBDASCII::Q].Down = true; }
			if (StatesASCII[KBDASCII::r].Down) { StatesASCII[KBDASCII::r].Down = false; StatesASCII[KBDASCII::R].Down = true; }
			if (StatesASCII[KBDASCII::s].Down) { StatesASCII[KBDASCII::s].Down = false; StatesASCII[KBDASCII::S].Down = true; }
			if (StatesASCII[KBDASCII::t].Down) { StatesASCII[KBDASCII::t].Down = false; StatesASCII[KBDASCII::T].Down = true; }
			if (StatesASCII[KBDASCII::u].Down) { StatesASCII[KBDASCII::u].Down = false; StatesASCII[KBDASCII::U].Down = true; }
			if (StatesASCII[KBDASCII::v].Down) { StatesASCII[KBDASCII::v].Down = false; StatesASCII[KBDASCII::V].Down = true; }
			if (StatesASCII[KBDASCII::w].Down) { StatesASCII[KBDASCII::w].Down = false; StatesASCII[KBDASCII::W].Down = true; }
			if (StatesASCII[KBDASCII::x].Down) { StatesASCII[KBDASCII::x].Down = false; StatesASCII[KBDASCII::X].Down = true; }
			if (StatesASCII[KBDASCII::y].Down) { StatesASCII[KBDASCII::y].Down = false; StatesASCII[KBDASCII::Y].Down = true; }
			if (StatesASCII[KBDASCII::z].Down) { StatesASCII[KBDASCII::z].Down = false; StatesASCII[KBDASCII::Z].Down = true; }
			if (StatesASCII[KBDASCII::One].Down) { StatesASCII[KBDASCII::One].Down = false; StatesASCII[KBDASCII::Exclamation].Down = true; }
			if (StatesASCII[KBDASCII::Two].Down) { StatesASCII[KBDASCII::Two].Down = false; StatesASCII[KBDASCII::At].Down = true; }
			if (StatesASCII[KBDASCII::Three].Down) { StatesASCII[KBDASCII::Three].Down = false; StatesASCII[KBDASCII::Pound].Down = true; }
			if (StatesASCII[KBDASCII::Four].Down) { StatesASCII[KBDASCII::Four].Down = false; StatesASCII[KBDASCII::Dollar].Down = true; }
			if (StatesASCII[KBDASCII::Five].Down) { StatesASCII[KBDASCII::Five].Down = false; StatesASCII[KBDASCII::Percent].Down = true; }
			if (StatesASCII[KBDASCII::Six].Down) { StatesASCII[KBDASCII::Six].Down = false; StatesASCII[KBDASCII::Hat].Down = true; }
			if (StatesASCII[KBDASCII::Seven].Down) { StatesASCII[KBDASCII::Seven].Down = false; StatesASCII[KBDASCII::Ampersand].Down = true; }
			if (StatesASCII[KBDASCII::Eight].Down) { StatesASCII[KBDASCII::Eight].Down = false; StatesASCII[KBDASCII::Asterisk].Down = true; }
			if (StatesASCII[KBDASCII::Nine].Down) { StatesASCII[KBDASCII::Nine].Down = false; StatesASCII[KBDASCII::ParanthesisL].Down = true; }
			if (StatesASCII[KBDASCII::Zero].Down) { StatesASCII[KBDASCII::Zero].Down = false; StatesASCII[KBDASCII::ParanthesisR].Down = true; }
			if (StatesASCII[KBDASCII::Dash].Down) { StatesASCII[KBDASCII::Dash].Down = false; StatesASCII[KBDASCII::Underscore].Down = true; }
			if (StatesASCII[KBDASCII::Plus].Down) { StatesASCII[KBDASCII::Plus].Down = false; StatesASCII[KBDASCII::Equal].Down = true; }
			if (StatesASCII[KBDASCII::BracketL].Down) { StatesASCII[KBDASCII::BracketL].Down = false; StatesASCII[KBDASCII::BraceL].Down = true; }
			if (StatesASCII[KBDASCII::BracketR].Down) { StatesASCII[KBDASCII::BracketR].Down = false; StatesASCII[KBDASCII::BraceR].Down = true; }
			if (StatesASCII[KBDASCII::BackSlash].Down) { StatesASCII[KBDASCII::BackSlash].Down = false; StatesASCII[KBDASCII::VerticalBar].Down = true; }
			if (StatesASCII[KBDASCII::Semicolon].Down) { StatesASCII[KBDASCII::Semicolon].Down = false; StatesASCII[KBDASCII::Colon].Down = true; }
			if (StatesASCII[KBDASCII::Apostrophe].Down) { StatesASCII[KBDASCII::Apostrophe].Down = false; StatesASCII[KBDASCII::Quote].Down = true; }
			if (StatesASCII[KBDASCII::Comma].Down) { StatesASCII[KBDASCII::Comma].Down = false; StatesASCII[KBDASCII::LessThan].Down = true; }
			if (StatesASCII[KBDASCII::Period].Down) { StatesASCII[KBDASCII::Period].Down = false; StatesASCII[KBDASCII::GreaterThan].Down = true; }
			if (StatesASCII[KBDASCII::FrontSlash].Down) { StatesASCII[KBDASCII::FrontSlash].Down = false; StatesASCII[KBDASCII::Question].Down = true; }
			if (StatesASCII[KBDASCII::BackTick].Down) { StatesASCII[KBDASCII::BackTick].Down = false; StatesASCII[KBDASCII::Tilde].Down = true; }
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

		if ((Index == KBDRAW::Shift) || (Index == KBDRAW::ShiftR))
		{
			// shift needs to immediately change the state of shift affected controls. Not the case for typing, but certainly for games.
			if (StatesASCII[KBDASCII::A].Down) { StatesASCII[KBDASCII::A].Down = false; StatesASCII[KBDASCII::a].Down = true; }
			if (StatesASCII[KBDASCII::B].Down) { StatesASCII[KBDASCII::B].Down = false; StatesASCII[KBDASCII::b].Down = true; }
			if (StatesASCII[KBDASCII::C].Down) { StatesASCII[KBDASCII::C].Down = false; StatesASCII[KBDASCII::c].Down = true; }
			if (StatesASCII[KBDASCII::D].Down) { StatesASCII[KBDASCII::D].Down = false; StatesASCII[KBDASCII::d].Down = true; }
			if (StatesASCII[KBDASCII::E].Down) { StatesASCII[KBDASCII::E].Down = false; StatesASCII[KBDASCII::e].Down = true; }
			if (StatesASCII[KBDASCII::F].Down) { StatesASCII[KBDASCII::F].Down = false; StatesASCII[KBDASCII::f].Down = true; }
			if (StatesASCII[KBDASCII::G].Down) { StatesASCII[KBDASCII::G].Down = false; StatesASCII[KBDASCII::g].Down = true; }
			if (StatesASCII[KBDASCII::H].Down) { StatesASCII[KBDASCII::H].Down = false; StatesASCII[KBDASCII::h].Down = true; }
			if (StatesASCII[KBDASCII::I].Down) { StatesASCII[KBDASCII::I].Down = false; StatesASCII[KBDASCII::i].Down = true; }
			if (StatesASCII[KBDASCII::J].Down) { StatesASCII[KBDASCII::J].Down = false; StatesASCII[KBDASCII::j].Down = true; }
			if (StatesASCII[KBDASCII::K].Down) { StatesASCII[KBDASCII::K].Down = false; StatesASCII[KBDASCII::k].Down = true; }
			if (StatesASCII[KBDASCII::L].Down) { StatesASCII[KBDASCII::L].Down = false; StatesASCII[KBDASCII::l].Down = true; }
			if (StatesASCII[KBDASCII::M].Down) { StatesASCII[KBDASCII::M].Down = false; StatesASCII[KBDASCII::m].Down = true; }
			if (StatesASCII[KBDASCII::N].Down) { StatesASCII[KBDASCII::N].Down = false; StatesASCII[KBDASCII::n].Down = true; }
			if (StatesASCII[KBDASCII::O].Down) { StatesASCII[KBDASCII::O].Down = false; StatesASCII[KBDASCII::o].Down = true; }
			if (StatesASCII[KBDASCII::P].Down) { StatesASCII[KBDASCII::P].Down = false; StatesASCII[KBDASCII::p].Down = true; }
			if (StatesASCII[KBDASCII::Q].Down) { StatesASCII[KBDASCII::Q].Down = false; StatesASCII[KBDASCII::q].Down = true; }
			if (StatesASCII[KBDASCII::R].Down) { StatesASCII[KBDASCII::R].Down = false; StatesASCII[KBDASCII::r].Down = true; }
			if (StatesASCII[KBDASCII::S].Down) { StatesASCII[KBDASCII::S].Down = false; StatesASCII[KBDASCII::s].Down = true; }
			if (StatesASCII[KBDASCII::T].Down) { StatesASCII[KBDASCII::T].Down = false; StatesASCII[KBDASCII::t].Down = true; }
			if (StatesASCII[KBDASCII::U].Down) { StatesASCII[KBDASCII::U].Down = false; StatesASCII[KBDASCII::u].Down = true; }
			if (StatesASCII[KBDASCII::V].Down) { StatesASCII[KBDASCII::V].Down = false; StatesASCII[KBDASCII::v].Down = true; }
			if (StatesASCII[KBDASCII::W].Down) { StatesASCII[KBDASCII::W].Down = false; StatesASCII[KBDASCII::w].Down = true; }
			if (StatesASCII[KBDASCII::X].Down) { StatesASCII[KBDASCII::X].Down = false; StatesASCII[KBDASCII::x].Down = true; }
			if (StatesASCII[KBDASCII::Y].Down) { StatesASCII[KBDASCII::Y].Down = false; StatesASCII[KBDASCII::y].Down = true; }
			if (StatesASCII[KBDASCII::Z].Down) { StatesASCII[KBDASCII::Z].Down = false; StatesASCII[KBDASCII::z].Down = true; }
			if (StatesASCII[KBDASCII::Exclamation].Down) { StatesASCII[KBDASCII::Exclamation].Down = false; StatesASCII[KBDASCII::One].Down = true; }
			if (StatesASCII[KBDASCII::At].Down) { StatesASCII[KBDASCII::At].Down = false; StatesASCII[KBDASCII::Two].Down = true; }
			if (StatesASCII[KBDASCII::Pound].Down) { StatesASCII[KBDASCII::Pound].Down = false; StatesASCII[KBDASCII::Three].Down = true; }
			if (StatesASCII[KBDASCII::Dollar].Down) { StatesASCII[KBDASCII::Dollar].Down = false; StatesASCII[KBDASCII::Four].Down = true; }
			if (StatesASCII[KBDASCII::Percent].Down) { StatesASCII[KBDASCII::Percent].Down = false; StatesASCII[KBDASCII::Five].Down = true; }
			if (StatesASCII[KBDASCII::Hat].Down) { StatesASCII[KBDASCII::Hat].Down = false; StatesASCII[KBDASCII::Six].Down = true; }
			if (StatesASCII[KBDASCII::Ampersand].Down) { StatesASCII[KBDASCII::Ampersand].Down = false; StatesASCII[KBDASCII::Seven].Down = true; }
			if (StatesASCII[KBDASCII::Asterisk].Down) { StatesASCII[KBDASCII::Asterisk].Down = false; StatesASCII[KBDASCII::Eight].Down = true; }
			if (StatesASCII[KBDASCII::ParanthesisL].Down) { StatesASCII[KBDASCII::ParanthesisL].Down = false; StatesASCII[KBDASCII::Nine].Down = true; }
			if (StatesASCII[KBDASCII::ParanthesisR].Down) { StatesASCII[KBDASCII::ParanthesisR].Down = false; StatesASCII[KBDASCII::Zero].Down = true; }
			if (StatesASCII[KBDASCII::Underscore].Down) { StatesASCII[KBDASCII::Underscore].Down = false; StatesASCII[KBDASCII::Dash].Down = true; }
			if (StatesASCII[KBDASCII::Equal].Down) { StatesASCII[KBDASCII::Equal].Down = false; StatesASCII[KBDASCII::Plus].Down = true; }
			if (StatesASCII[KBDASCII::BraceL].Down) { StatesASCII[KBDASCII::BraceL].Down = false; StatesASCII[KBDASCII::BracketL].Down = true; }
			if (StatesASCII[KBDASCII::BraceR].Down) { StatesASCII[KBDASCII::BraceR].Down = false; StatesASCII[KBDASCII::BracketR].Down = true; }
			if (StatesASCII[KBDASCII::VerticalBar].Down) { StatesASCII[KBDASCII::VerticalBar].Down = false; StatesASCII[KBDASCII::BackSlash].Down = true; }
			if (StatesASCII[KBDASCII::Colon].Down) { StatesASCII[KBDASCII::Colon].Down = false; StatesASCII[KBDASCII::Semicolon].Down = true; }
			if (StatesASCII[KBDASCII::Quote].Down) { StatesASCII[KBDASCII::Quote].Down = false; StatesASCII[KBDASCII::Apostrophe].Down = true; }
			if (StatesASCII[KBDASCII::LessThan].Down) { StatesASCII[KBDASCII::LessThan].Down = false; StatesASCII[KBDASCII::Comma].Down = true; }
			if (StatesASCII[KBDASCII::GreaterThan].Down) { StatesASCII[KBDASCII::GreaterThan].Down = false; StatesASCII[KBDASCII::Period].Down = true; }
			if (StatesASCII[KBDASCII::Question].Down) { StatesASCII[KBDASCII::Question].Down = false; StatesASCII[KBDASCII::FrontSlash].Down = true; }
			if (StatesASCII[KBDASCII::Tilde].Down) { StatesASCII[KBDASCII::Tilde].Down = false; StatesASCII[KBDASCII::BackTick].Down = true; }
		}

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
