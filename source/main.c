/*
 * MIT License
 *
 * Copyright (c) 2018 - 2023 Phil Badura
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

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>

#include "resource.h"

#include "interface.h"

#define ARRAY_LENGTH(a) (sizeof(a) / sizeof(a[0]))

HANDLE hThread = NULL;
HANDLE hMutex = NULL;
BOOL bRun = FALSE;

void handleGamepads()
{
	while (bRun)
	{
		WaitForSingleObject(hMutex, INFINITE);

		gamepadsUpdate();

		Sleep(10);

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

			int iIndex = (int)(LOWORD(wParam) - IDM_GAMEPAD);

			if (iIndex >= 0 && iIndex < gamepadsCount())
			{
				gamepadToggle(iIndex);
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

				for (int iIndex = 0; iIndex < gamepadsCount(); iIndex++)
				{
					if (gamepadIsConnected(iIndex))
					{
						anyConnected = TRUE;

						TCHAR szName[] = TEXT("Gamepad 0");

						szName[8] = szName[8] + (TCHAR)iIndex;

						AppendMenu(hMenu, gamepadIsEnabled(iIndex) ? MF_CHECKED : MF_UNCHECKED, (UINT_PTR)(IDM_GAMEPAD + iIndex), szName);
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

	bRun = TRUE;

	gamepadsInitialize();

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

		bRun = FALSE;

		ReleaseMutex(hMutex);
	}

	WaitForSingleObject(hThread, INFINITE);

exit:;

	CloseHandle(hThread);

	gamepadsTerminate();

	CloseHandle(hMutex);

	if (hWnd)
	{
		Shell_NotifyIcon(NIM_DELETE, &notifyIconData);
	}

	return (int)msg.wParam;
}
