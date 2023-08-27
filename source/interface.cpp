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

#include "interface.h"

#include "gamepad.hpp"

gp::GamepadPtr gamepads[4] = { };

int gamepadsCount()
{
	return static_cast<int>(sizeof(gamepads) / sizeof(gp::GamepadPtr));
}

void gamepadsInitialize()
{
	for (int iIndex = 0; iIndex < gamepadsCount(); iIndex++)
	{
		gamepads[iIndex] = gp::makeDefault(iIndex, false);
	}
}

void gamepadsTerminate()
{
	for (int iIndex = 0; iIndex < gamepadsCount(); iIndex++)
	{
		gamepads[iIndex] = nullptr;
	}
}

void gamepadsUpdate()
{
	for (int iIndex = 0; iIndex < gamepadsCount(); iIndex++)
	{
		gamepads[iIndex]->update();
	}
}

int gamepadIsConnected(const int iIndex)
{
	return static_cast<int>(gamepads[iIndex]->isConnected());
}

int gamepadIsEnabled(const int iIndex)
{
	return static_cast<int>(gamepads[iIndex]->isEnabled());
}

void gamepadToggle(const int iIndex)
{
	gamepads[iIndex]->toggle();
}
