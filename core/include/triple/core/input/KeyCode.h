#ifndef KEY_CODE_H
#define KEY_CODE_H

namespace triple::core {
	enum class KeyCode : int {
		Unknown = -1,

		// Letters — must stay sequential A..Z for convertKey arithmetic
		A = 0, B, C, D, E, F, G, H, I, J, K, L, M,
		N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

		// Digits — must stay sequential Num0..Num9
		Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

		// Function keys — must stay sequential F1..F25
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25,

		// Numpad digits — must stay sequential KP0..KP9
		KP0, KP1, KP2, KP3, KP4, KP5, KP6, KP7, KP8, KP9,
		KPDecimal, KPDivide, KPMultiply, KPSubtract, KPAdd, KPEnter, KPEqual,

		// Arrow keys
		Left, Right, Up, Down,

		// Modifier keys
		LeftShift, RightShift,
		LeftCtrl, RightCtrl,
		LeftAlt, RightAlt,
		LeftSuper, RightSuper,

		// Navigation
		Insert, Delete, Home, End, PageUp, PageDown,

		// Whitespace / editing
		Space, Tab, Backspace, Enter,

		// Misc
		Escape, CapsLock, PrintScreen, ScrollLock, Pause, Menu, NumLock,

		// Punctuation
		Apostrophe,    // '
		Comma,         // ,
		Minus,         // -
		Period,        // .
		Slash,         // /
		Semicolon,     // ;
		Equal,         // =
		LeftBracket,   // [
		Backslash,     // '\'
		RightBracket,  // ]
		GraveAccent,   // `

		// International
		World1, World2,
	};
}

#endif // KEY_CODE_H
