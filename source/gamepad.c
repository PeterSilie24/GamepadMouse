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

#include "gamepad.h"

Gamepad gamepadCreate(DWORD dwIndex)
{
	if (dwIndex > 3)
	{
		return NULL;
	}

	Gamepad gamepad = (Gamepad)calloc(1, sizeof(GamepadStruct));

	if (gamepad)
	{
		gamepad->dwIndex = dwIndex;

		gamepadUpdate(gamepad);
	}

	return gamepad;
}

void gamepadDestroy(Gamepad gamepad)
{
	if (gamepad)
	{
		free(gamepad);
	}
}

void gamepadUpdate(Gamepad gamepad)
{
	if (gamepad)
	{
		if (!gamepad->connected)
		{
			if ((GetTickCount64() - gamepad->ticks) > 250)
			{
				gamepad->ticks = GetTickCount64();
			}
			else
			{
				return;
			}
		}
		else
		{
			gamepad->ticks = GetTickCount64();
		}

		gamepad->connected = TRUE;

		if (XInputGetState(gamepad->dwIndex, &(gamepad->state)) == ERROR_DEVICE_NOT_CONNECTED)
		{
			gamepad->connected = FALSE;
		}
	}
}

DWORD gamepadGetIndex(Gamepad gamepad)
{
	if (gamepad)
	{
		return gamepad->dwIndex;
	}

	return (DWORD)(~0);
}

BOOL gamepadIsConnected(Gamepad gamepad)
{
	if (gamepad)
	{
		return gamepad->connected;
	}

	return FALSE;
}

BOOL gamepadIsButton(Gamepad gamepad, GamepadButton button)
{
	if (gamepad)
	{
		return (gamepad->state.Gamepad.wButtons & button) == button;
	}

	return FALSE;
}

double gamepadGetLeftTrigger(Gamepad gamepad)
{
	if (gamepad)
	{
		return (double)(gamepad->state.Gamepad.bLeftTrigger) / 255.0;
	}

	return 0.0;
}

double gamepadGetRightTrigger(Gamepad gamepad)
{
	if (gamepad)
	{
		return (double)(gamepad->state.Gamepad.bRightTrigger) / 255.0;
	}

	return 0.0;
}

double gamepadGetLeftThumbX(Gamepad gamepad)
{
	if (gamepad)
	{
		if (gamepad->state.Gamepad.sThumbLX < 0)
		{
			return (double)(gamepad->state.Gamepad.sThumbLX) / 32768.0;
		}
		
		return (double)(gamepad->state.Gamepad.sThumbLX) / 32767.0;
	}

	return 0.0;
}

double gamepadGetLeftThumbY(Gamepad gamepad)
{
	if (gamepad)
	{
		if (gamepad->state.Gamepad.sThumbLY < 0)
		{
			return (double)(gamepad->state.Gamepad.sThumbLY) / 32768.0;
		}

		return (double)(gamepad->state.Gamepad.sThumbLY) / 32767.0;
	}

	return 0.0;
}

void gamepadGetLeftThumb(Gamepad gamepad, double* pX, double* pY)
{
	if (gamepad && pX && pY)
	{
		*pX = gamepadGetLeftThumbX(gamepad);

		*pY = gamepadGetLeftThumbY(gamepad);
	}
}

double gamepadGetRightThumbX(Gamepad gamepad)
{
	if (gamepad)
	{
		if (gamepad->state.Gamepad.sThumbRX < 0)
		{
			return (double)(gamepad->state.Gamepad.sThumbRX) / 32768.0;
		}

		return (double)(gamepad->state.Gamepad.sThumbRX) / 32767.0;
	}

	return 0.0;
}

double gamepadGetRightThumbY(Gamepad gamepad)
{
	if (gamepad)
	{
		if (gamepad->state.Gamepad.sThumbRY < 0)
		{
			return (double)(gamepad->state.Gamepad.sThumbRY) / 32768.0;
		}

		return (double)(gamepad->state.Gamepad.sThumbRY) / 32767.0;
	}

	return 0.0;
}

void gamepadGetRightThumb(Gamepad gamepad, double* pX, double* pY)
{
	if (gamepad && pX && pY)
	{
		*pX = gamepadGetRightThumbX(gamepad);

		*pY = gamepadGetRightThumbY(gamepad);
	}
}

void gamepadVibrate(Gamepad gamepad, double leftMotorSpeed, double rightMotorSpeed)
{
	if (gamepad)
	{
		XINPUT_VIBRATION vibration;

		vibration.wLeftMotorSpeed = (WORD)(max(min(leftMotorSpeed * 65535.0, 65535.0), 0.0));

		vibration.wRightMotorSpeed = (WORD)(max(min(rightMotorSpeed * 65535.0, 65535.0), 0.0));

		XInputSetState(gamepad->dwIndex, &vibration);
	}
}

void deadzoneCoords(double* pX, double* pY, double deadzone)
{
	if (pX && pY)
	{
		double len = sqrt((*pX) * (*pX) + (*pY) * (*pY));

		if (len > deadzone && len > 0.0)
		{
			*pX /= len;

			*pY /= len;

			len = (len - deadzone) / (1.0 - deadzone);

			*pX *= min(len, 1.0);

			*pY *= min(len, 1.0);
		}
		else
		{
			*pX = 0.0;

			*pY = 0.0;
		}
	}
}
