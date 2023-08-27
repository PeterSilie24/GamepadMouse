/*
 * MIT License
 *
 * Copyright (c) 2023 Phil Badura
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

namespace key
{
	struct Key
	{
		typedef enum : int
		{
			MouseButtonLeft,
			MouseButtonRight,
			Cancel,
			MouseButtonMiddle,
			MouseButtonX1,
			MouseButtonX2,
			Back,
			Tab,
			Clear,
			Return,
			Shift,
			Control,
			Menu,
			Pause,
			CapitalLock,
			IMEKana,
			IMEHanguel,
			IMEHangul,
			IMEOn,
			IMEJunja,
			IMEFinal,
			IMEHanja,
			IMEKanji,
			IMEOff,
			Escape,
			IMEConvert,
			IMENonConvert,
			IMEAccept,
			IMEModeChange,
			Space,
			Prior,
			Next,
			End,
			Home,
			Left,
			Up,
			Right,
			Down,
			Select,
			Print,
			Execute,
			Snapshot,
			Insert,
			Delete,
			Help,
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
			WindowsLeft,
			WindowsRight,
			Applications,
			Sleep,
			NumPad0,
			NumPad1,
			NumPad2,
			NumPad3,
			NumPad4,
			NumPad5,
			NumPad6,
			NumPad7,
			NumPad8,
			NumPad9,
			Multiply,
			Add,
			Separator,
			Subtract,
			Decimal,
			Divide,
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
			F13,
			F14,
			F15,
			F16,
			F17,
			F18,
			F19,
			F20,
			F21,
			F22,
			F23,
			F24,
			NumLock,
			ScrollLock,
			ShiftLeft,
			ShiftRight,
			ControlLeft,
			ControlRight,
			MenuLeft,
			MenuRight,
			BrowserBack,
			BrowserForward,
			BrowserRefresh,
			BrowserStop,
			BrowserSearch,
			BrowserFavorites,
			BrowserHome,
			VolumeMute,
			VolumeDown,
			VolumeUp,
			MediaNextTrack,
			MediaPreviousTrack,
			MediaStop,
			MediaPlayPause,
			LaunchMail,
			LaunchMediaSelect,
			LaunchApplication1,
			LaunchApplication2,
			OEM1,
			OEMPlus,
			OEMComma,
			OEMMinus,
			OEMPeriod,
			OEM2,
			OEM3,
			OEM4,
			OEM5,
			OEM6,
			OEM7,
			OEM8,
			OEM102,
			ProcessKey,
			Packet,
			Attn,
			CrSel,
			ExSel,
			ErEOF,
			Play,
			Zoom,
			NoName,
			PA1,
			OEMClear,
			Count
		} Name;
	};

	extern void press(const Key::Name key);

	extern void release(const Key::Name key);

	extern void onScreenKeyboardOpen();

	extern void onScreenKeyboardClose();

	extern void onScreenKeyboardToggle();
	
	extern void shortcut();

	template <typename... Names>
	void shortcut(const Key::Name key, const Names... next)
	{
		press(key);

		shortcut(next...);

		release(key);
	}

	extern void switchWindows();

	extern void takeScreenshot();
}
