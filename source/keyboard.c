/*
 * MIT License
 *
 * Copyright (c) 2018 Phil Badura
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

#include "keyboard.h"

HANDLE hOSK = NULL;

void openOnScreenKeyboard()
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

void closeOnScreenKeyboard()
{
	if (hOSK)
	{
		TerminateProcess(hOSK, 0);

		CloseHandle(hOSK);

		hOSK = 0;
	}
}

void toggleOnScreenKeyboard()
{
	if (hOSK)
	{
		DWORD dwExitCode = 0;

		GetExitCodeProcess(hOSK, &dwExitCode);

		if (dwExitCode != STILL_ACTIVE)
		{
			closeOnScreenKeyboard();

			openOnScreenKeyboard();
		}
		else
		{
			closeOnScreenKeyboard();
		}
	}
	else
	{
		openOnScreenKeyboard();
	}
}

void keyboardPressKey(WORD wVk)
{
	INPUT input;

	ZeroMemory(&input, sizeof(input));

	input.type = INPUT_KEYBOARD;

	input.ki.wVk = wVk;

	SendInput(1, &input, sizeof(input));
}

void keyboardReleaseKey(WORD wVk)
{
	INPUT input;

	ZeroMemory(&input, sizeof(input));

	input.type = INPUT_KEYBOARD;

	input.ki.wVk = wVk;
	input.ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput(1, &input, sizeof(input));
}

void keyboardPressReturn()
{
	keyboardPressKey(VK_RETURN);
}

void keyboardReleaseReturn()
{
	keyboardReleaseKey(VK_RETURN);
}

void keyboardPressControl()
{
	keyboardPressKey(VK_CONTROL);
}

void keyboardReleaseControl()
{
	keyboardReleaseKey(VK_CONTROL);
}

void keyboardPressUp()
{
	keyboardPressKey(VK_UP);
}

void keyboardReleaseUp()
{
	keyboardReleaseKey(VK_UP);
}

void keyboardPressDown()
{
	keyboardPressKey(VK_DOWN);
}

void keyboardReleaseDown()
{
	keyboardReleaseKey(VK_DOWN);
}

void keyboardPressLeft()
{
	keyboardPressKey(VK_LEFT);
}

void keyboardReleaseLeft()
{
	keyboardReleaseKey(VK_LEFT);
}

void keyboardPressRight()
{
	keyboardPressKey(VK_RIGHT);
}

void keyboardReleaseRight()
{
	keyboardReleaseKey(VK_RIGHT);
}

void keyboardShortcut(WORD wVk1, WORD wVk2)
{
	keyboardPressKey(wVk1);

	keyboardPressKey(wVk2);

	keyboardReleaseKey(wVk2);

	keyboardReleaseKey(wVk1);
}

void keyboardShortcutExtended(WORD wVk1, WORD wVk2, WORD wVk3)
{
	keyboardPressKey(wVk1);

	keyboardPressKey(wVk2);

	keyboardPressKey(wVk3);

	keyboardReleaseKey(wVk3);

	keyboardReleaseKey(wVk2);

	keyboardReleaseKey(wVk1);
}

void switchWindows()
{
	keyboardShortcut(VK_MENU, VK_ESCAPE);
}

void takeScreenshot()
{
	keyboardShortcut(VK_LWIN, VK_SNAPSHOT);
}
