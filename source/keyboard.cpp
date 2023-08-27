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

#include "keyboard.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>

namespace key
{
	const WORD wKeyMap[] =
	{
		VK_LBUTTON,
		VK_RBUTTON,
		VK_CANCEL,
		VK_MBUTTON,
		VK_XBUTTON1,
		VK_XBUTTON2,
		VK_BACK,
		VK_TAB,
		VK_CLEAR,
		VK_RETURN,
		VK_SHIFT,
		VK_CONTROL,
		VK_MENU,
		VK_PAUSE,
		VK_CAPITAL,
		VK_KANA,
		VK_HANGUL,
		VK_HANGUL,
		VK_IME_ON,
		VK_JUNJA,
		VK_FINAL,
		VK_HANJA,
		VK_KANJI,
		VK_IME_OFF,
		VK_ESCAPE,
		VK_CONVERT,
		VK_NONCONVERT,
		VK_ACCEPT,
		VK_MODECHANGE,
		VK_SPACE,
		VK_PRIOR,
		VK_NEXT,
		VK_END,
		VK_HOME,
		VK_LEFT,
		VK_UP,
		VK_RIGHT,
		VK_DOWN,
		VK_SELECT,
		VK_PRINT,
		VK_EXECUTE,
		VK_SNAPSHOT,
		VK_INSERT,
		VK_DELETE,
		VK_HELP,
		0x30,
		0x31,
		0x32,
		0x33,
		0x34,
		0x35,
		0x36,
		0x37,
		0x38,
		0x39,
		0x41,
		0x42,
		0x43,
		0x44,
		0x45,
		0x46,
		0x47,
		0x48,
		0x49,
		0x4A,
		0x4B,
		0x4C,
		0x4D,
		0x4E,
		0x4F,
		0x50,
		0x51,
		0x52,
		0x53,
		0x54,
		0x55,
		0x56,
		0x57,
		0x58,
		0x59,
		0x5A,
		VK_LWIN,
		VK_RWIN,
		VK_APPS,
		VK_SLEEP,
		VK_NUMPAD0,
		VK_NUMPAD1,
		VK_NUMPAD2,
		VK_NUMPAD3,
		VK_NUMPAD4,
		VK_NUMPAD5,
		VK_NUMPAD6,
		VK_NUMPAD7,
		VK_NUMPAD8,
		VK_NUMPAD9,
		VK_MULTIPLY,
		VK_ADD,
		VK_SEPARATOR,
		VK_SUBTRACT,
		VK_DECIMAL,
		VK_DIVIDE,
		VK_F1,
		VK_F2,
		VK_F3,
		VK_F4,
		VK_F5,
		VK_F6,
		VK_F7,
		VK_F8,
		VK_F9,
		VK_F10,
		VK_F11,
		VK_F12,
		VK_F13,
		VK_F14,
		VK_F15,
		VK_F16,
		VK_F17,
		VK_F18,
		VK_F19,
		VK_F20,
		VK_F21,
		VK_F22,
		VK_F23,
		VK_F24,
		VK_NUMLOCK,
		VK_SCROLL,
		VK_LSHIFT,
		VK_RSHIFT,
		VK_LCONTROL,
		VK_RCONTROL,
		VK_LMENU,
		VK_RMENU,
		VK_BROWSER_BACK,
		VK_BROWSER_FORWARD,
		VK_BROWSER_REFRESH,
		VK_BROWSER_STOP,
		VK_BROWSER_SEARCH,
		VK_BROWSER_FAVORITES,
		VK_BROWSER_HOME,
		VK_VOLUME_MUTE,
		VK_VOLUME_DOWN,
		VK_VOLUME_UP,
		VK_MEDIA_NEXT_TRACK,
		VK_MEDIA_PREV_TRACK,
		VK_MEDIA_STOP,
		VK_MEDIA_PLAY_PAUSE,
		VK_LAUNCH_MAIL,
		VK_LAUNCH_MEDIA_SELECT,
		VK_LAUNCH_APP1,
		VK_LAUNCH_APP2,
		VK_OEM_1,
		VK_OEM_PLUS,
		VK_OEM_COMMA,
		VK_OEM_MINUS,
		VK_OEM_PERIOD,
		VK_OEM_2,
		VK_OEM_3,
		VK_OEM_4,
		VK_OEM_5,
		VK_OEM_6,
		VK_OEM_7,
		VK_OEM_8,
		VK_OEM_102,
		VK_PROCESSKEY,
		VK_PACKET,
		VK_ATTN,
		VK_CRSEL,
		VK_EXSEL,
		VK_EREOF,
		VK_PLAY,
		VK_ZOOM,
		VK_NONAME,
		VK_PA1,
		VK_OEM_CLEAR
	};

	HANDLE hOSK = NULL;

	void press(const Key::Name key)
	{
		if (key < Key::Name::Count)
		{
			INPUT input;

			ZeroMemory(&input, sizeof(input));

			input.type = INPUT_KEYBOARD;

			input.ki.wVk = wKeyMap[key];

			SendInput(1, &input, sizeof(input));
		}
	}

	void release(const Key::Name key)
	{
		if (key < Key::Name::Count)
		{
			INPUT input;

			ZeroMemory(&input, sizeof(input));

			input.type = INPUT_KEYBOARD;

			input.ki.wVk = wKeyMap[key];
			input.ki.dwFlags = KEYEVENTF_KEYUP;

			SendInput(1, &input, sizeof(input));
		}
	}

	void onScreenKeyboardOpen()
	{
		PVOID oldValue = NULL;

		Wow64DisableWow64FsRedirection(&oldValue);

		SHELLEXECUTEINFO shellExecuteInfo;

		ZeroMemory(&shellExecuteInfo, sizeof(shellExecuteInfo));

		shellExecuteInfo.cbSize = sizeof(shellExecuteInfo);
		shellExecuteInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		shellExecuteInfo.lpVerb = TEXT("open");
		shellExecuteInfo.lpFile = TEXT("osk.exe");
		shellExecuteInfo.nShow = SW_SHOWDEFAULT;

		if (ShellExecuteEx(&shellExecuteInfo))
		{
			hOSK = shellExecuteInfo.hProcess;
		}

		Wow64RevertWow64FsRedirection(oldValue);
	}

	void onScreenKeyboardClose()
	{
		if (hOSK)
		{
			TerminateProcess(hOSK, 0);

			CloseHandle(hOSK);

			hOSK = NULL;
		}
	}

	void onScreenKeyboardToggle()
	{
		if (hOSK)
		{
			DWORD dwExitCode = 0;

			GetExitCodeProcess(hOSK, &dwExitCode);

			if (dwExitCode != STILL_ACTIVE)
			{
				onScreenKeyboardClose();

				onScreenKeyboardOpen();
			}
			else
			{
				onScreenKeyboardClose();
			}
		}
		else
		{
			onScreenKeyboardOpen();
		}
	}

	void shortcut()
	{

	}

	void switchWindows()
	{
		shortcut(Key::Menu, Key::Escape);
	}

	void takeScreenshot()
	{
		shortcut(Key::WindowsLeft, Key::Snapshot);
	}
}
