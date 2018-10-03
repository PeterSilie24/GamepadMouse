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

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <Windows.h>
#include <Xinput.h>

#pragma comment(lib, "XInput.lib")

typedef struct _Gamepad
{
	DWORD dwIndex;
	BOOL connected;
	ULONGLONG ticks;
	XINPUT_STATE state;
} * Gamepad, GamepadStruct;

typedef enum _GamepadButton
{
	GAMEPAD_DPAD_UP = XINPUT_GAMEPAD_DPAD_UP,
	GAMEPAD_DPAD_DOWN = XINPUT_GAMEPAD_DPAD_DOWN,
	GAMEPAD_DPAD_LEFT = XINPUT_GAMEPAD_DPAD_LEFT,
	GAMEPAD_DPAD_RIGHT = XINPUT_GAMEPAD_DPAD_RIGHT,
	GAMEPAD_START = XINPUT_GAMEPAD_START,
	GAMEPAD_BACK = XINPUT_GAMEPAD_BACK,
	GAMEPAD_LEFT_THUMB = XINPUT_GAMEPAD_LEFT_THUMB,
	GAMEPAD_RIGHT_THUMB = XINPUT_GAMEPAD_RIGHT_THUMB,
	GAMEPAD_LEFT_SHOULDER = XINPUT_GAMEPAD_LEFT_SHOULDER,
	GAMEPAD_RIGHT_SHOULDER = XINPUT_GAMEPAD_RIGHT_SHOULDER,
	GAMEPAD_A = XINPUT_GAMEPAD_A,
	GAMEPAD_B = XINPUT_GAMEPAD_B,
	GAMEPAD_X = XINPUT_GAMEPAD_X,
	GAMEPAD_Y = XINPUT_GAMEPAD_Y
} GamepadButton;

extern Gamepad gamepadCreate(DWORD dwIndex);

extern void gamepadDestroy(Gamepad gamepad);

extern void gamepadUpdate(Gamepad gamepad);

extern DWORD gamepadGetIndex(Gamepad gamepad);

extern BOOL gamepadIsConnected(Gamepad gamepad);

extern BOOL gamepadIsButton(Gamepad gamepad, GamepadButton button);

extern double gamepadGetLeftTrigger(Gamepad gamepad);

extern double gamepadGetRightTrigger(Gamepad gamepad);

extern double gamepadGetLeftThumbX(Gamepad gamepad);

extern double gamepadGetLeftThumbY(Gamepad gamepad);

extern void gamepadGetLeftThumb(Gamepad gamepad, double* pX, double* pY);

extern double gamepadGetRightThumbX(Gamepad gamepad);

extern double gamepadGetRightThumbY(Gamepad gamepad);

extern void gamepadGetRightThumb(Gamepad gamepad, double* pX, double* pY);

extern void gamepadVibrate(Gamepad gamepad, double leftMotorSpeed, double rightMotorSpeed);

extern void deadzoneCoords(double* pX, double* pY, double deadzone);
