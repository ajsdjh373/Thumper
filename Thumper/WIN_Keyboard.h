/*
Author: Nathan Dunn

This is the revamped keyboard handling class.
Functions from this class are called by a window's lpfnWndProc function,
ie the message handler for the window.

This is an improved version of Chili's original keyboard input system.
This version features less classes, no runtime memory allocation,
and a shorter call stack both for setting key states and retrieving
key states. Everything should also be in stack memory. Furthermore,
this version supports almost every standard key on a 103 key keyboard
with functions to manage input how the application wants to.

Note: the shorter call stack wasn't measured, but by eliminating the
need for InputHandler (or InputManager before that), there are less
layers of abstraction required to handle the keyboard.

Keys that are not supported include:
Manufacturer specific keys (FN, macro keys)
Special functions for the function keys (volume control, next/previous/sleep)
Possibly right WinKey (needs tested)
Num pad always behaves as if numlock is off

Due to limitations in the hardware, the following are indistinguishable:
Enter, numpad enter
Shift vs left shift (same for alt, ctrl)

There are two interfaces for the keyboard: Raw and ASCII. Raw records on what
keys are pressed, an entry in the raw table maps 1:1 with a physical key.
ASCII records only the state of ascii characters, so each ascii entry doesn't
necessarilly map 1:1 to a key on the keyboard, and not all keys are included
in the ASCII table. Only common place ASCII characters are tracked. Use the
Raw interface for keybinds. Use the ASCII interface for typing.

A word on CheckRaw and CheckASCII------------------------------------
The return value of these functions is a unique pointer to a Key object.
The functions call make_unique on a key object from the corresponding
array, then return that unique pointer. Instead could have returned a c
pointer, or the struct itself. I could even write access functions to
the individual struct members. The unique pointer has more memory overhead
than any other option due to copying the struct and making a pointer.
However, it is memory safe and prevents editing of the keystate in an
unintended way (unlike a c pointer). Unlike returning the struct, the
unique pointer can be moved at a lower cost. Note that right now the struct
and the unique pointer are both 8 bytes, but in the future the struct will
probably be made bigger.

TL;DR: the unique pointer offers the best safety and lowest movement cost.
*/

#pragma once
#include <array>
#include <functional>
#include <memory>

namespace WIN
{
	class Keyboard;
}

/*
Alias namespace for raw key input
Naming exception: due to the use and nature of this, it is allowed to be global
*/
namespace KBDRAW
{
	enum KBDRAW
	{
		NotKey,
		Excluded,
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		W,
		V,
		X,
		Y,
		Z,
		Std0,
		Std1,
		Std2,
		Std3,
		Std4,
		Std5,
		Std6,
		Std7,
		Std8,
		Std9,
		Tab,
		CapsLock,
		Enter,
		Space,
		Backspace,
		Menu,
		WinKey,
		Shift,
		ShiftR,
		Alt,
		AltR,
		Ctrl,
		CtrlR,
		BackTick,
		Dash,
		Equals,
		BracketL,
		BracketR,
		SemiColon,
		Apostrophe,
		Comma,
		Period,
		FrontSlash,
		BackSlash,
		Esc,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		PrintScreen,
		ScrollLock,
		Sleep,
		Insert,
		Home,
		PageUp,
		Del,
		End,
		PageDown,
		Up,
		Down,
		Left,
		Right,
		NumLock,
		Divide,
		Multiply,
		Subtract,
		Dot,
		Plus,
		Separator,
		Num0,
		Num1,
		Num2,
		Num3,
		Num4,
		Num5,
		Num6,
		Num7,
		Num8,
		Num9,
		EnumLength
	};

	// raw keyboard key state
	struct Key
	{
		bool Down = false;
		unsigned int Total = 0;
	};

}

/*
Alias namespace for ASCII key input
Naming exception: due to the use and nature of this, it is allowed to be global
*/
namespace KBDASCII
{
	enum KBDASCII
	{
		NotKey,
		Excluded,
		Zero,
		One,
		Two,
		Three,
		Four,
		Five,
		Six,
		Seven,
		Eight,
		Nine,
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		a,
		b,
		c,
		d,
		e,
		f,
		g,
		h,
		i,
		j,
		k,
		l,
		m,
		n,
		o,
		p,
		q,
		r,
		s,
		t,
		u,
		v,
		w,
		x,
		y,
		z,
		Delete,
		Backspace,
		Enter,
		Space,
		Tab,
		Exclamation,
		At,
		Pound,
		Dollar,
		Percent,
		Hat,
		Ampersand,
		Asterisk,
		ParanthesisL,
		ParanthesisR,
		BackTick,
		Dash,
		Equal,
		BracketL,
		BracketR,
		Semicolon,
		Apostrophe,
		Comma,
		Period,
		FrontSlash,
		BackSlash,
		Tilde,
		Underscore,
		Plus,
		BraceL,
		BraceR,
		Colon,
		Quote,
		LessThan,
		GreaterThan,
		Question,
		VerticalBar,
		EnumLenth
	};

	// ascii keyboard key state (identical to raw)
	struct Key
	{
		bool Down = false;
		unsigned int Total = 0;
	};

};

/*
Main keyboard class
*/
class WIN::Keyboard
{
public:
	// public member functions
	Keyboard();

	// management functions
	void ClearState();
	void ClearASCII();
	void ClearRawTotal(int RawKey);
	void ClearASCIITotal(int ASCIIKey);
	void OnKeyDown(int lParam, int wParam);
	void OnKeyUp(int lParam, int wParam);
	void SetFocus(bool Input);

	// key check functions
	std::unique_ptr<KBDRAW::Key> CheckRaw(int RawKey);
	std::unique_ptr<KBDASCII::Key> CheckASCII(int ASCIIKey);
	bool CheckFocus();
	int CheckLastRaw();
	int CheckLastASCII();

	// for use in the Raw to ASCII hash table in Keyboard
	// pass a KBDRAW entry, get a KBDASCII entry
	int HashBlank(int Key);
	int HashASCIILetter(int Key);
	int HashASCIIStdNum(int Key);
	int HashASCIISymbol(int Key);
	int HashASCIIControl(int Key);
	int HashASCIINumpad(int Key);

private:
	// private member functions

public:
	// public member variables

private:
	// private member variables
	int LastRaw;
	int LastASCII;
	std::array<KBDRAW::Key, KBDRAW::EnumLength> StatesRaw;
	std::array<KBDASCII::Key, KBDASCII::EnumLenth> StatesASCII;
	bool Focus;

	// variables for hash functions
	int OffsetL;
	int OffsetU;
	int OffsetSN;
	int OffsetSNS;
	int OffsetNN;
	int OffsetS;
	int OffsetSS;

	// for debugging
	size_t BytesInShort;
	size_t BytesInInt;
	size_t BytesInLong;
	size_t BytesInLongLong;

private:
	// hash tables

	/*
	Very sensitive!
	The length of these hash tables corresponds to the length of the enum entries that follow!
	*/

	// hash table for mapping wParam to keys (represented via global enum entries)
	// access with a virtual keycode obtained from wParam, get the index for the raw data structure
	const std::array<int, 255> RawHashTable =
	{
		/*
		Maps to					Index	Virtual key code
		*/
		KBDRAW::NotKey,			//	0x00	undefined
		KBDRAW::Excluded,		//	0x01	LBUTTON
		KBDRAW::Excluded,		//	0x02	RBUTTON
		KBDRAW::Excluded,		//	0x03	CANCEL
		KBDRAW::Excluded,		//	0x04	MBUTTON
		KBDRAW::Excluded,		//	0x05	XBUTTON1
		KBDRAW::Excluded,		//	0x06	XBUTTON2
		KBDRAW::Excluded,		//	0x07	undefined
		KBDRAW::Backspace,		//	0x08	BACKSPACE
		KBDRAW::Tab,			//	0x09	TAB
		KBDRAW::Excluded,		//	0x0A	reserved
		KBDRAW::Excluded,		//	0x0B	reserved
		KBDRAW::Num5,			//	0x0C	CLEAR
		KBDRAW::Enter,			//	0x0D	RETURN
		KBDRAW::Excluded,		//	0x0E	undefined
		KBDRAW::Excluded,		//	0x0F	undefined
		KBDRAW::Shift,			//	0x10	SHIFT
		KBDRAW::Ctrl,			//	0x11	CONTROL
		KBDRAW::Alt,			//	0x12	MENU
		KBDRAW::Sleep,			//	0x13	PAUSE
		KBDRAW::CapsLock,		//	0x14	CAPITAL
		KBDRAW::Excluded,		//	0x15	HANGUL
		KBDRAW::Excluded,		//	0x16	IME_ON
		KBDRAW::Excluded,		//	0x17	JUNJA
		KBDRAW::Excluded,		//	0x18	FINAL
		KBDRAW::Excluded,		//	0x19	HANJA
		KBDRAW::Excluded,		//	0x1A	IME_OFF
		KBDRAW::Esc,			//	0x1B	ESCAPE
		KBDRAW::Excluded,		//	0x1C	CONVERT
		KBDRAW::Excluded,		//	0x1D	NONCONVERT
		KBDRAW::Excluded,		//	0x1E	ACCEPT
		KBDRAW::Excluded,		//	0x1F	MODECHANGE
		KBDRAW::Space,			//	0x20	SPACE
		KBDRAW::PageUp,			//	0x21	PRIOR
		KBDRAW::PageDown,		//	0x22	NEXT
		KBDRAW::End,			//	0x23	END
		KBDRAW::Home,			//	0x24	HOME
		KBDRAW::Left,			//	0x25	LEFT
		KBDRAW::Up,				//	0x26	UP
		KBDRAW::Right,			//	0x27	RIGHT
		KBDRAW::Down,			//	0x28	DOWN
		KBDRAW::Excluded,		//	0x29	SELECT
		KBDRAW::Excluded,		//	0x2A	PRINT
		KBDRAW::Excluded,		//	0x2B	EXECUTE
		KBDRAW::PrintScreen,	//	0x2C	SNAPSHOT
		KBDRAW::Insert,			//	0x2D	INSERT
		KBDRAW::Del,			//	0x2E	DELETE
		KBDRAW::Excluded,		//	0x2F	HELP
		KBDRAW::Std0,			//	0x30	0
		KBDRAW::Std1,			//	0x31	1
		KBDRAW::Std2,			//	0x32	2
		KBDRAW::Std3,			//	0x33	3
		KBDRAW::Std4,			//	0x34	4
		KBDRAW::Std5,			//	0x35	5
		KBDRAW::Std6,			//	0x36	6
		KBDRAW::Std7,			//	0x37	7
		KBDRAW::Std8,			//	0x38	8
		KBDRAW::Std9,			//	0x39	9
		KBDRAW::Excluded,		//	0x3A	undefined
		KBDRAW::Excluded,		//	0x3B	undefined
		KBDRAW::Excluded,		//	0x3C	undefined
		KBDRAW::Excluded,		//	0x3D	undefined
		KBDRAW::Excluded,		//	0x3E	undefined
		KBDRAW::Excluded,		//	0x3F	undefined
		KBDRAW::Excluded,		//	0x40	undefined
		KBDRAW::A,				//	0x41	A
		KBDRAW::B,				//	0x42	B
		KBDRAW::C,				//	0x43	C
		KBDRAW::D,				//	0x44	D
		KBDRAW::E,				//	0x45	E
		KBDRAW::F,				//	0x46	F
		KBDRAW::G,				//	0x47	G
		KBDRAW::H,				//	0x48	H
		KBDRAW::I,				//	0x49	I
		KBDRAW::J,				//	0x4A	J
		KBDRAW::K,				//	0x4B	K
		KBDRAW::L,				//	0x4C	L
		KBDRAW::M,				//	0x4D	M
		KBDRAW::N,				//	0x4E	N
		KBDRAW::O,				//	0x4F	O
		KBDRAW::P,				//	0x50	P
		KBDRAW::Q,				//	0x51	Q
		KBDRAW::R,				//	0x52	R
		KBDRAW::S,				//	0x53	S
		KBDRAW::T,				//	0x54	T
		KBDRAW::U,				//	0x55	U
		KBDRAW::V,				//	0x56	V
		KBDRAW::W,				//	0x57	W
		KBDRAW::X,				//	0x58	X
		KBDRAW::Y,				//	0x59	Y
		KBDRAW::Z,				//	0x5A	Z
		KBDRAW::WinKey,			//	0x5B	LWIN
		KBDRAW::WinKey,			//	0x5C	RWIN
		KBDRAW::Menu,			//	0x5D	APPS
		KBDRAW::Excluded,		//	0x5E	reserved
		KBDRAW::Excluded,		//	0x5F	SLEEP
		KBDRAW::Num0,			//	0x60	NUMPAD0
		KBDRAW::Num1,			//	0x61	NUMPAD1
		KBDRAW::Num2,			//	0x62	NUMPAD2
		KBDRAW::Num3,			//	0x63	NUMPAD3
		KBDRAW::Num4,			//	0x64	NUMPAD4
		KBDRAW::Num5,			//	0x65	NUMPAD5
		KBDRAW::Num6,			//	0x66	NUMPAD6
		KBDRAW::Num7,			//	0x67	NUMPAD7
		KBDRAW::Num8,			//	0x68	NUMPAD8
		KBDRAW::Num9,			//	0x69	NUMPAD9
		KBDRAW::Multiply,		//	0x6A	MULTIPLY
		KBDRAW::Plus,			//	0x6B	ADD
		KBDRAW::Separator,		//	0x6C	SEPARATOR
		KBDRAW::Subtract,		//	0x6D	SUBTRACT
		KBDRAW::Dot,			//	0x6E	DECIMAL
		KBDRAW::Divide,			//	0x6F	DIVIDE
		KBDRAW::F1,				//	0x70	F1
		KBDRAW::F2,				//	0x71	F2
		KBDRAW::F3,				//	0x72	F3
		KBDRAW::F4,				//	0x73	F4
		KBDRAW::F5,				//	0x74	F5
		KBDRAW::F6,				//	0x75	F6
		KBDRAW::F7,				//	0x76	F7
		KBDRAW::F8,				//	0x77	F8
		KBDRAW::F9,				//	0x78	F9
		KBDRAW::F10,			//	0x79	F10
		KBDRAW::F11,			//	0x7A	F11
		KBDRAW::F12,			//	0x7B	F12
		KBDRAW::Excluded,		//	0x7C	F13
		KBDRAW::Excluded,		//	0x7D	F14
		KBDRAW::Excluded,		//	0x7E	F15
		KBDRAW::Excluded,		//	0x7F	F16
		KBDRAW::Excluded,		//	0x80	F17
		KBDRAW::Excluded,		//	0x81	F18
		KBDRAW::Excluded,		//	0x82	F19
		KBDRAW::Excluded,		//	0x83	F20
		KBDRAW::Excluded,		//	0x84	F21
		KBDRAW::Excluded,		//	0x85	F22
		KBDRAW::Excluded,		//	0x86	F23
		KBDRAW::Excluded,		//	0x87	F24
		KBDRAW::Excluded,		//	0x88	undefined
		KBDRAW::Excluded,		//	0x89	undefined
		KBDRAW::Excluded,		//	0x8A	undefined
		KBDRAW::Excluded,		//	0x8B	undefined
		KBDRAW::Excluded,		//	0x8C	undefined
		KBDRAW::Excluded,		//	0x8D	undefined
		KBDRAW::Excluded,		//	0x8E	undefined
		KBDRAW::Excluded,		//	0x8F	undefined
		KBDRAW::NumLock,		//	0x90	NUMLOCK
		KBDRAW::ScrollLock,		//	0x91	SCROLL
		KBDRAW::Excluded,		//	0x92	oem specific
		KBDRAW::Excluded,		//	0x93	oem specific
		KBDRAW::Excluded,		//	0x94	oem specific
		KBDRAW::Excluded,		//	0x95	oem specific
		KBDRAW::Excluded,		//	0x96	oem specific
		KBDRAW::Excluded,		//	0x97	undefined
		KBDRAW::Excluded,		//	0x98	undefined
		KBDRAW::Excluded,		//	0x99	undefined
		KBDRAW::Excluded,		//	0x9A	undefined
		KBDRAW::Excluded,		//	0x9B	undefined
		KBDRAW::Excluded,		//	0x9C	undefined
		KBDRAW::Excluded,		//	0x9D	undefined
		KBDRAW::Excluded,		//	0x9E	undefined
		KBDRAW::Excluded,		//	0x9F	undefined
		KBDRAW::Shift,			//	0xA0	LSHIFT
		KBDRAW::ShiftR,			//	0xA1	RSHIFT
		KBDRAW::Ctrl,			//	0xA2	LCONTROL
		KBDRAW::CtrlR,			//	0xA3	RCONTROL
		KBDRAW::Alt,			//	0xA4	LMENU
		KBDRAW::AltR,			//	0xA5	RMENU
		KBDRAW::Excluded,		//	0xA6	BROWSER_BACK
		KBDRAW::Excluded,		//	0xA7	BROWSER_FORWARD
		KBDRAW::Excluded,		//	0xA8	BROWSER_REFRESH
		KBDRAW::Excluded,		//	0xA9	BROWSER_STOP
		KBDRAW::Excluded,		//	0xAA	BROWSER_SEARCH
		KBDRAW::Excluded,		//	0xAB	BROWSER_FAVORITES
		KBDRAW::Excluded,		//	0xAC	BROWSER_HOME
		KBDRAW::Excluded,		//	0xAD	VOLUME_MUTE
		KBDRAW::Excluded,		//	0xAE	VOLUME_DOWN
		KBDRAW::Excluded,		//	0xAF	VOLUME_UP
		KBDRAW::Excluded,		//	0xB0	MEDIA_NEXT_TRACK
		KBDRAW::Excluded,		//	0xB1	MEDIA_PREV_TRACK
		KBDRAW::Excluded,		//	0xB2	MEDIA_STOP
		KBDRAW::Excluded,		//	0xB3	MEDIA_PLAY_PAUSE
		KBDRAW::Excluded,		//	0xB4	LAUNCH_MAIL
		KBDRAW::Excluded,		//	0xB5	LAUNCH_MEDIA_SELECT
		KBDRAW::Excluded,		//	0xB6	LAUNCH_APP1
		KBDRAW::Excluded,		//	0xB7	lAUNCH_APP2
		KBDRAW::Excluded,		//	0xB8	reserved
		KBDRAW::Excluded,		//	0xB9	reserved
		KBDRAW::SemiColon,		//	0xBA	US Standard Keyboard ;:
		KBDRAW::Plus,			//	0xBB	OEM_PLUS
		KBDRAW::Comma,			//	0xBC	OEM_COMMA
		KBDRAW::Dash,			//	0xBD	OEM_MINUS
		KBDRAW::Period,			//	0xBE	OEM_PERIOD
		KBDRAW::FrontSlash,		//	0xBF	US Standard Keyboard /?
		KBDRAW::BackTick,		//	0xC0	US Standard Keyboard `~
		KBDRAW::Excluded,		//	0xC1	reserved
		KBDRAW::Excluded,		//	0xC2	reserved
		KBDRAW::Excluded,		//	0xC3	reserved
		KBDRAW::Excluded,		//	0xC4	reserved
		KBDRAW::Excluded,		//	0xC5	reserved
		KBDRAW::Excluded,		//	0xC6	reserved
		KBDRAW::Excluded,		//	0xC7	reserved
		KBDRAW::Excluded,		//	0xC8	reserved
		KBDRAW::Excluded,		//	0xC9	reserved
		KBDRAW::Excluded,		//	0xCA	reserved
		KBDRAW::Excluded,		//	0xCB	reserved
		KBDRAW::Excluded,		//	0xCC	reserved
		KBDRAW::Excluded,		//	0xCD	reserved
		KBDRAW::Excluded,		//	0xCE	reserved
		KBDRAW::Excluded,		//	0xCF	reserved
		KBDRAW::Excluded,		//	0xD0	reserved
		KBDRAW::Excluded,		//	0xD1	reserved
		KBDRAW::Excluded,		//	0xD2	reserved
		KBDRAW::Excluded,		//	0xD3	reserved
		KBDRAW::Excluded,		//	0xD4	reserved
		KBDRAW::Excluded,		//	0xD5	reserved
		KBDRAW::Excluded,		//	0xD6	reserved
		KBDRAW::Excluded,		//	0xD7	reserved
		KBDRAW::Excluded,		//	0xD8	undefined
		KBDRAW::Excluded,		//	0xD9	undefined
		KBDRAW::Excluded,		//	0xDA	undefined
		KBDRAW::BracketL,		//	0xDB	US Standard Keyboard [{
		KBDRAW::BackSlash,		//	0xDC	US Standard Keyboard \|
		KBDRAW::BracketR,		//	0xDD	US Standard Keyboard ]}
		KBDRAW::Apostrophe,		//	0xDE	US Standard Keyboard '"
		KBDRAW::Excluded,		//	0xDF	oem specific
		KBDRAW::Excluded,		//	0xE0	reserved
		KBDRAW::Excluded,		//	0xE1	oem specific
		KBDRAW::Excluded,		//	0xE2	oem specific
		KBDRAW::Excluded,		//	0xE3	oem specific
		KBDRAW::Excluded,		//	0xE4	oem specific
		KBDRAW::Excluded,		//	0xE5	PROCESSKEY
		KBDRAW::Excluded,		//	0xE6	oem specific
		KBDRAW::Excluded,		//	0xE7	PACKET
		KBDRAW::Excluded,		//	0xE8	undefined
		KBDRAW::Excluded,		//	0xE9	oem specific
		KBDRAW::Excluded,		//	0xEA	oem specific
		KBDRAW::Excluded,		//	0xEB	oem specific
		KBDRAW::Excluded,		//	0xEC	oem specific
		KBDRAW::Excluded,		//	0xED	oem specific
		KBDRAW::Excluded,		//	0xEE	oem specific
		KBDRAW::Excluded,		//	0xEF	oem specific
		KBDRAW::Excluded,		//	0xF0	oem specific
		KBDRAW::Excluded,		//	0xF1	oem specific
		KBDRAW::Excluded,		//	0xF2	oem specific
		KBDRAW::Excluded,		//	0xF3	oem specific
		KBDRAW::Excluded,		//	0xF4	oem specific
		KBDRAW::Excluded,		//	0xF5	oem specific
		KBDRAW::Excluded,		//	0xF6	ATTN
		KBDRAW::Excluded,		//	0xF7	CRSEL
		KBDRAW::Excluded,		//	0xF8	EXSEL
		KBDRAW::Excluded,		//	0xF9	EREOF
		KBDRAW::Excluded,		//	0xFA	PLAY
		KBDRAW::Excluded,		//	0xFB	ZOOM
		KBDRAW::Excluded,		//	0xFC	NONAME
		KBDRAW::Excluded,		//	0xFD	PA1
		KBDRAW::Excluded		//	0xFE	OEM_CLEAR
	};

	// hash table for managing ASCII characters from key presses
	// access with enum entry from KBDRAW, get a function pointer that will set the ASCII data structure appropriately
	const std::array<std::function<int(Keyboard*, int)>, KBDRAW::EnumLength> ASCIIHashTable =
	{
		&Keyboard::HashBlank,			//	NotKey
		&Keyboard::HashBlank,			//	Excluded
		&Keyboard::HashASCIILetter,		//	A
		&Keyboard::HashASCIILetter,		//	B
		&Keyboard::HashASCIILetter,		//	C
		&Keyboard::HashASCIILetter,		//	D
		&Keyboard::HashASCIILetter,		//	E
		&Keyboard::HashASCIILetter,		//	F
		&Keyboard::HashASCIILetter,		//	G
		&Keyboard::HashASCIILetter,		//	H
		&Keyboard::HashASCIILetter,		//	I
		&Keyboard::HashASCIILetter,		//	J
		&Keyboard::HashASCIILetter,		//	K
		&Keyboard::HashASCIILetter,		//	L
		&Keyboard::HashASCIILetter,		//	M
		&Keyboard::HashASCIILetter,		//	N
		&Keyboard::HashASCIILetter,		//	O
		&Keyboard::HashASCIILetter,		//	P
		&Keyboard::HashASCIILetter,		//	Q
		&Keyboard::HashASCIILetter,		//	R
		&Keyboard::HashASCIILetter,		//	S
		&Keyboard::HashASCIILetter,		//	T
		&Keyboard::HashASCIILetter,		//	U
		&Keyboard::HashASCIILetter,		//	W
		&Keyboard::HashASCIILetter,		//	V
		&Keyboard::HashASCIILetter,		//	X
		&Keyboard::HashASCIILetter,		//	Y
		&Keyboard::HashASCIILetter,		//	Z
		&Keyboard::HashASCIIStdNum,		//	Std0
		&Keyboard::HashASCIIStdNum,		//	Std1
		&Keyboard::HashASCIIStdNum,		//	Std2
		&Keyboard::HashASCIIStdNum,		//	Std3
		&Keyboard::HashASCIIStdNum,		//	Std4
		&Keyboard::HashASCIIStdNum,		//	Std5
		&Keyboard::HashASCIIStdNum,		//	Std6
		&Keyboard::HashASCIIStdNum,		//	Std7
		&Keyboard::HashASCIIStdNum,		//	Std8
		&Keyboard::HashASCIIStdNum,		//	Std9
		&Keyboard::HashASCIIControl,	//	Tab
		&Keyboard::HashBlank,			//	CapsLock
		&Keyboard::HashASCIIControl,	//	Enter
		&Keyboard::HashASCIIControl,	//	Space
		&Keyboard::HashASCIIControl,	//	Backspace
		&Keyboard::HashBlank,			//	Menu
		&Keyboard::HashBlank,			//	WinKey
		&Keyboard::HashBlank,			//	Shift
		&Keyboard::HashBlank,			//	ShiftR
		&Keyboard::HashBlank,			//	Alt
		&Keyboard::HashBlank,			//	AltR
		&Keyboard::HashBlank,			//	Ctrl
		&Keyboard::HashBlank,			//	CtrlR
		&Keyboard::HashASCIISymbol,		//	BackTick
		&Keyboard::HashASCIISymbol,		//	Dash
		&Keyboard::HashASCIISymbol,		//	Equals
		&Keyboard::HashASCIISymbol,		//	BracketL
		&Keyboard::HashASCIISymbol,		//	BracketR
		&Keyboard::HashASCIISymbol,		//	SemiColon
		&Keyboard::HashASCIISymbol,		//	Apostrophe
		&Keyboard::HashASCIISymbol,		//	Comma
		&Keyboard::HashASCIISymbol,		//	Period
		&Keyboard::HashASCIISymbol,		//	FrontSlash
		&Keyboard::HashASCIISymbol,		//	BackSlash
		&Keyboard::HashBlank,			//	Esc
		&Keyboard::HashBlank,			//	F1
		&Keyboard::HashBlank,			//	F2
		&Keyboard::HashBlank,			//	F3
		&Keyboard::HashBlank,			//	F4
		&Keyboard::HashBlank,			//	F5
		&Keyboard::HashBlank,			//	F6
		&Keyboard::HashBlank,			//	F7
		&Keyboard::HashBlank,			//	F8
		&Keyboard::HashBlank,			//	F9
		&Keyboard::HashBlank,			//	F10
		&Keyboard::HashBlank,			//	F11
		&Keyboard::HashBlank,			//	F12
		&Keyboard::HashBlank,			//	PrintScreen
		&Keyboard::HashBlank,			//	ScrollLock
		&Keyboard::HashBlank,			//	Sleep
		&Keyboard::HashBlank,			//	Insert
		&Keyboard::HashBlank,			//	Home
		&Keyboard::HashBlank,			//	PageUp
		&Keyboard::HashBlank,			//	Del
		&Keyboard::HashBlank,			//	End
		&Keyboard::HashBlank,			//	PageDown
		&Keyboard::HashBlank,			//	Up
		&Keyboard::HashBlank,			//	Down
		&Keyboard::HashBlank,			//	Left
		&Keyboard::HashBlank,			//	Right
		&Keyboard::HashBlank,			//	NumLock
		&Keyboard::HashASCIINumpad,		//	Divide
		&Keyboard::HashASCIINumpad,		//	Multiply
		&Keyboard::HashASCIINumpad,		//	Subtract
		&Keyboard::HashASCIINumpad,		//	Dot
		&Keyboard::HashASCIINumpad,		//	Plus
		&Keyboard::HashASCIINumpad,		//	Separator
		&Keyboard::HashASCIINumpad,		//	Num0
		&Keyboard::HashASCIINumpad,		//	Num1
		&Keyboard::HashASCIINumpad,		//	Num2
		&Keyboard::HashASCIINumpad,		//	Num3
		&Keyboard::HashASCIINumpad,		//	Num4
		&Keyboard::HashASCIINumpad,		//	Num5
		&Keyboard::HashASCIINumpad,		//	Num6
		&Keyboard::HashASCIINumpad,		//	Num7
		&Keyboard::HashASCIINumpad,		//	Num8
		&Keyboard::HashASCIINumpad		//	Num9
	};

	// hash table for generating ascii chars from the keyboard state
	const std::array<char, KBDASCII::EnumLenth> CharHashTable =
	{
		'\x87',			//	NotKey		Representing this with the double dagger
		'\x87',			//	Excluded	Representing this with the double dagger
		'0',			//	Zero
		'1',			//	One
		'2',			//	Two
		'3',			//	Three
		'4',			//	Four
		'5',			//	Five
		'6',			//	Six
		'7',			//	Seven
		'8',			//	Eight
		'9',			//	Nine
		'A',			//	A
		'B',			//	B
		'C',			//	C
		'D',			//	D
		'E',			//	E
		'F',			//	F
		'G',			//	G
		'H',			//	H
		'I',			//	I
		'J',			//	J
		'K',			//	K
		'L',			//	L
		'M',			//	M
		'N',			//	N
		'O',			//	O
		'P',			//	P
		'Q',			//	Q
		'R',			//	R
		'S',			//	S
		'T',			//	T
		'U',			//	U
		'V',			//	V
		'W',			//	W
		'X',			//	X
		'Y',			//	Y
		'Z',			//	Z
		'a',			//	a
		'b',			//	b
		'c',			//	c
		'd',			//	d
		'e',			//	e
		'f',			//	f
		'g',			//	g
		'h',			//	h
		'i',			//	i
		'j',			//	j
		'k',			//	k
		'l',			//	l
		'm',			//	m
		'n',			//	n
		'o',			//	o
		'p',			//	p
		'q',			//	q
		'r',			//	r
		's',			//	s
		't',			//	t
		'u',			//	u
		'v',			//	v
		'w',			//	w
		'x',			//	x
		'y',			//	y
		'z',			//	z
		'\x7F',			//	Delete
		'\b',			//	Backspace
		'\n',			//	Enter
		' ',			//	Space
		'\t',			//	Tab
		'!',			//	Exclamation
		'@',			//	At
		'#',			//	Pound
		'$',			//	Dollar
		'%',			//	Percent
		'^',			//	Hat
		'&',			//	Ampersand
		'*',			//	Asterisk
		'(',			//	ParanthesisL
		')',			//	ParanthesisR
		'`',			//	BackTick
		'-',			//	Dash
		'=',			//	Equal
		'[',			//	BracketL
		']',			//	BracketR
		';',			//	Semicolon
		'\'',			//	Apostrophe
		',',			//	Comma
		'.',			//	Period
		'/',			//	FrontSlash
		'\\',			//	BackSlash
		'~',			//	Tilde
		'_',			//	Underscore
		'+',			//	Plus
		'{',			//	BraceL
		'}',			//	BraceR
		':',			//	Colon
		'"',			//	Quote
		'<',			//	LessThan
		'>',			//	GreaterThan
		'?',			//	Question
		'|'				//	VerticalBar
	};

};


