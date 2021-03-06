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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <Windows.h>

#include "resource.h"

#include "gamepad.h"
#include "mouse.h"
#include "keyboard.h"

#define ARRAY_LENGTH(a) (sizeof(a) / sizeof(a[0]))

typedef struct _GamepadState
{
	Gamepad gamepad;

	BOOL enabled;
	BOOL stateEnable;

	double dx;
	double dy;
	double sdx;
	double sdy;

	BOOL stateUp;
	BOOL stateDown;
	BOOL stateLeft;
	BOOL stateRight;

	BOOL stateA;
	BOOL stateB;
	BOOL stateX;
	BOOL stateY;

	BOOL stateLeftThumb;
	BOOL stateRightThumb;

	BOOL stateLeftShoulder;
	BOOL stateRightShoulder;
} GamepadState;

GamepadState gamepadStates[4];

void handleButton(Gamepad gamepad, GamepadButton button, BOOL* stateFlag, void(*pressFunction)(), void(*releaseFunction)())
{
	if (stateFlag)
	{
		if (gamepadIsButton(gamepad, button) && !(*stateFlag))
		{
			*stateFlag = TRUE;

			if (pressFunction)
			{
				pressFunction();
			}
		}
		else if (!gamepadIsButton(gamepad, button) && *stateFlag)
		{
			*stateFlag = FALSE;

			if (releaseFunction)
			{
				releaseFunction();
			}
		}
	}
}

void handleGamepadState(GamepadState* pGamepadState)
{
	if (pGamepadState)
	{
		gamepadUpdate(pGamepadState->gamepad);

		if (gamepadIsConnected(pGamepadState->gamepad))
		{
			if (gamepadIsButton(pGamepadState->gamepad, GAMEPAD_BACK | GAMEPAD_START) && !pGamepadState->stateEnable)
			{
				pGamepadState->stateEnable = TRUE;

				gamepadVibrate(pGamepadState->gamepad, 0.15, 0.15);
			}
			else if (!gamepadIsButton(pGamepadState->gamepad, GAMEPAD_BACK | GAMEPAD_START) && pGamepadState->stateEnable)
			{
				pGamepadState->stateEnable = FALSE;

				pGamepadState->enabled = pGamepadState->enabled ? FALSE : TRUE;

				gamepadVibrate(pGamepadState->gamepad, 0.0, 0.0);
			}

			if (pGamepadState->enabled)
			{
				double x = gamepadGetLeftThumbX(pGamepadState->gamepad);
				double y = gamepadGetLeftThumbY(pGamepadState->gamepad);

				deadzoneCoords(&x, &y, 0.2);

				double len = sqrt(x * x + y * y);

				if (len > 0.0)
				{
					pGamepadState->dx += x * len * 15.0;
					pGamepadState->dy += y * len * 15.0;

					double intpartX = 0.0;
					double intpartY = 0.0;

					pGamepadState->dx = modf(pGamepadState->dx, &intpartX);
					pGamepadState->dy = modf(pGamepadState->dy, &intpartY);

					mouseMove((LONG)intpartX, -(LONG)intpartY);
				}

				x = gamepadGetRightThumbX(pGamepadState->gamepad);
				y = gamepadGetRightThumbY(pGamepadState->gamepad);

				deadzoneCoords(&x, &y, 0.2);

				y -= gamepadGetLeftTrigger(pGamepadState->gamepad);
				y += gamepadGetRightTrigger(pGamepadState->gamepad);

				len = sqrt(x * x + y * y);

				if (len > 0.0)
				{
					pGamepadState->sdx += x * len * 30.0;
					pGamepadState->sdy += y * len * 30.0;

					double intpartX = 0.0;
					double intpartY = 0.0;

					pGamepadState->sdx = modf(pGamepadState->sdx, &intpartX);
					pGamepadState->sdy = modf(pGamepadState->sdy, &intpartY);

					mouseScroll((LONG)intpartX, (LONG)intpartY);
				}

				handleButton(pGamepadState->gamepad, GAMEPAD_A, &(pGamepadState->stateA), mousePressLeft, mouseReleaseLeft);

				handleButton(pGamepadState->gamepad, GAMEPAD_B, &(pGamepadState->stateB), mousePressRight, mouseReleaseRight);

				handleButton(pGamepadState->gamepad, GAMEPAD_X, &(pGamepadState->stateX), keyboardPressReturn, keyboardReleaseReturn);

				handleButton(pGamepadState->gamepad, GAMEPAD_Y, &(pGamepadState->stateY), NULL, toggleOnScreenKeyboard);

				handleButton(pGamepadState->gamepad, GAMEPAD_DPAD_UP, &(pGamepadState->stateUp), keyboardPressUp, keyboardReleaseUp);

				handleButton(pGamepadState->gamepad, GAMEPAD_DPAD_DOWN, &(pGamepadState->stateDown), keyboardPressDown, keyboardReleaseDown);

				handleButton(pGamepadState->gamepad, GAMEPAD_DPAD_LEFT, &(pGamepadState->stateLeft), keyboardPressLeft, keyboardReleaseLeft);

				handleButton(pGamepadState->gamepad, GAMEPAD_DPAD_RIGHT, &(pGamepadState->stateRight), keyboardPressRight, keyboardReleaseRight);

				handleButton(pGamepadState->gamepad, GAMEPAD_LEFT_THUMB, &(pGamepadState->stateLeftThumb), mousePressMiddle, mouseReleaseMiddle);

				handleButton(pGamepadState->gamepad, GAMEPAD_RIGHT_THUMB, &(pGamepadState->stateRightThumb), keyboardPressControl, keyboardReleaseControl);

				handleButton(pGamepadState->gamepad, GAMEPAD_LEFT_SHOULDER, &(pGamepadState->stateLeftShoulder), NULL, switchWindows);

				handleButton(pGamepadState->gamepad, GAMEPAD_RIGHT_SHOULDER, &(pGamepadState->stateRightShoulder), NULL, takeScreenshot);
			}
		}
	}
}

HANDLE hThread = NULL;
HANDLE hMutex = NULL;
BOOL run = FALSE;

void handleGamepads()
{
	{
		WaitForSingleObject(hMutex, INFINITE);

		ZeroMemory(&gamepadStates, sizeof(gamepadStates));

		for (size_t i = 0; i < ARRAY_LENGTH(gamepadStates); i++)
		{
			gamepadStates[i].gamepad = gamepadCreate((DWORD)i);
		}

		ReleaseMutex(hMutex);
	}

	while (TRUE)
	{
		WaitForSingleObject(hMutex, INFINITE);

		if (!run)
		{
			ReleaseMutex(hMutex);

			break;
		}

		for (size_t i = 0; i < ARRAY_LENGTH(gamepadStates); i++)
		{
			handleGamepadState(gamepadStates + i);
		}

		ReleaseMutex(hMutex);

		Sleep(10);
	}

	{
		WaitForSingleObject(hMutex, INFINITE);

		for (size_t i = 0; i < ARRAY_LENGTH(gamepadStates); i++)
		{
			gamepadDestroy(gamepadStates[i].gamepad);
		}

		ReleaseMutex(hMutex);
	}
}

TCHAR szAppTitle[128];
HWND hWnd = HWND_DESKTOP;

#define WM_USER_SHELLICON WM_USER
#define IDM_EXIT 100
#define IDM_GAMEPAD 101

LRESULT CALLBACK wndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uiMsg)
	{
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDM_EXIT:
		{
			DestroyWindow(hWnd);

			break;
		}
		case IDM_GAMEPAD + 0:
		case IDM_GAMEPAD + 1:
		case IDM_GAMEPAD + 2:
		case IDM_GAMEPAD + 3:
		{
			WaitForSingleObject(hMutex, INFINITE);

			size_t index = (size_t)(LOWORD(wParam) - IDM_GAMEPAD);

			if (index >= 0 && index < ARRAY_LENGTH(gamepadStates))
			{
				gamepadStates[index].enabled = gamepadStates[index].enabled ? FALSE : TRUE;
			}

			ReleaseMutex(hMutex);

			break;
		}
		}

		break;
	}
	case WM_USER_SHELLICON:
	{
		switch (LOWORD(lParam))
		{
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		{
			HMENU hMenu = CreatePopupMenu();

			BOOL anyConnected = FALSE;

			{
				WaitForSingleObject(hMutex, INFINITE);

				for (size_t i = 0; i < ARRAY_LENGTH(gamepadStates); i++)
				{
					if (gamepadIsConnected(gamepadStates[i].gamepad))
					{
						anyConnected = TRUE;

						TCHAR szName[] = TEXT("Gamepad 0");

						szName[8] = szName[8] + (TCHAR)i;

						AppendMenu(hMenu, gamepadStates[i].enabled ? MF_CHECKED : MF_UNCHECKED, (UINT_PTR)(IDM_GAMEPAD + i), szName);
					}
				}

				ReleaseMutex(hMutex);
			}

			if (anyConnected)
			{
				AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
			}

			AppendMenu(hMenu, MF_STRING, IDM_EXIT, TEXT("Exit"));

			POINT point;

			GetCursorPos(&point);

			SetForegroundWindow(hWnd);

			TrackPopupMenu(hMenu, TPM_TOPALIGN | TPM_LEFTALIGN, point.x, point.y, 0, hWnd, NULL);

			DestroyMenu(hMenu);

			break;
		}
		}

		break;
	}
	case WM_CLOSE:
	{
		DestroyWindow(hWnd);

		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(EXIT_SUCCESS);

		break;
	}
	}

	return DefWindowProc(hWnd, uiMsg, wParam, lParam);
}

void assertError()
{
	DWORD dwLastError = GetLastError();

	LPTSTR lpErrorMessage = NULL;

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, dwLastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpErrorMessage, 0, NULL);

	if (lpErrorMessage)
	{
		MessageBox(hWnd, lpErrorMessage, szAppTitle, MB_ICONERROR | MB_OK);

		LocalFree(lpErrorMessage);
	}
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	msg.wParam = EXIT_FAILURE;

	LoadString(hInstance, IDS_APP_TITLE, szAppTitle, ARRAY_LENGTH(szAppTitle));

	WNDCLASS wndClass;

	ZeroMemory(&wndClass, sizeof(wndClass));
	
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = wndProc;
	wndClass.lpszClassName = TEXT("wndClass");
	wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP));

	if (!RegisterClass(&wndClass))
	{
		assertError();

		goto exit;
	}

	hWnd = CreateWindow(wndClass.lpszClassName, szAppTitle, WS_POPUPWINDOW, 0, 0, 100, 100, HWND_DESKTOP, NULL, hInstance, NULL);

	if (!hWnd)
	{
		assertError();

		goto exit;
	}

	NOTIFYICONDATA notifyIconData;

	ZeroMemory(&notifyIconData, sizeof(notifyIconData));

	notifyIconData.cbSize = sizeof(notifyIconData);
	notifyIconData.hWnd = hWnd;
	notifyIconData.uID = 0;
	notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	notifyIconData.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP));
	notifyIconData.uCallbackMessage = WM_USER_SHELLICON;

	LoadString(hInstance, IDS_APP_TITLE, notifyIconData.szTip, ARRAY_LENGTH(notifyIconData.szTip));

	Shell_NotifyIcon(NIM_ADD, &notifyIconData);

	hMutex = CreateMutex(NULL, FALSE, NULL);

	if (!hMutex)
	{
		assertError();

		goto exit;
	}

	run = TRUE;

	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)handleGamepads, NULL, 0, NULL);

	if (!hThread)
	{
		assertError();

		goto exit;
	}

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);

		DispatchMessage(&msg);
	}

	{
		WaitForSingleObject(hMutex, INFINITE);

		run = FALSE;

		ReleaseMutex(hMutex);
	}

	WaitForSingleObject(hThread, INFINITE);

exit:;

	CloseHandle(hThread);

	CloseHandle(hMutex);

	if (hWnd)
	{
		Shell_NotifyIcon(NIM_DELETE, &notifyIconData);
	}

	return (int)msg.wParam;
}
