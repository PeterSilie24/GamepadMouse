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

#include "mouse.h"

void mouseMove(LONG dx, LONG dy)
{
	INPUT input;

	ZeroMemory(&input, sizeof(input));

	input.type = INPUT_MOUSE;

	input.mi.dx = dx;
	input.mi.dy = dy;
	input.mi.dwFlags = MOUSEEVENTF_MOVE;

	SendInput(1, &input, sizeof(input));
}

void mouseScroll(LONG dx, LONG dy)
{
	INPUT input;

	ZeroMemory(&input, sizeof(input));

	input.type = INPUT_MOUSE;

	input.mi.mouseData = (DWORD)dy;
	input.mi.dwFlags = MOUSEEVENTF_WHEEL;

	SendInput(1, &input, sizeof(input));

	ZeroMemory(&input, sizeof(input));

	input.type = INPUT_MOUSE;

	input.mi.mouseData = (DWORD)dx;
	input.mi.dwFlags = MOUSEEVENTF_HWHEEL;

	SendInput(1, &input, sizeof(input));
}

void mousePressLeft()
{
	INPUT input;

	ZeroMemory(&input, sizeof(input));

	input.type = INPUT_MOUSE;

	input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

	SendInput(1, &input, sizeof(input));
}

void mouseReleaseLeft()
{
	INPUT input;

	ZeroMemory(&input, sizeof(input));

	input.type = INPUT_MOUSE;

	input.mi.dwFlags = MOUSEEVENTF_LEFTUP;

	SendInput(1, &input, sizeof(input));
}

void mousePressRight()
{
	INPUT input;

	ZeroMemory(&input, sizeof(input));

	input.type = INPUT_MOUSE;

	input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

	SendInput(1, &input, sizeof(input));
}

void mouseReleaseRight()
{
	INPUT input;

	ZeroMemory(&input, sizeof(input));

	input.type = INPUT_MOUSE;

	input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;

	SendInput(1, &input, sizeof(input));
}

void mousePressMiddle()
{
	INPUT input;

	ZeroMemory(&input, sizeof(input));

	input.type = INPUT_MOUSE;

	input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;

	SendInput(1, &input, sizeof(input));
}

void mouseReleaseMiddle()
{
	INPUT input;

	ZeroMemory(&input, sizeof(input));

	input.type = INPUT_MOUSE;

	input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;

	SendInput(1, &input, sizeof(input));
}
