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

#include "mouse.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace mouse
{
	void moveX(const double dx)
	{
		static double dxRemainder = 0.0;

		dxRemainder += dx;

		if (dxRemainder != 0.0)
		{
			INPUT input;

			ZeroMemory(&input, sizeof(input));

			input.type = INPUT_MOUSE;

			input.mi.dx = static_cast<LONG>(dxRemainder);
			input.mi.dwFlags = MOUSEEVENTF_MOVE;

			dxRemainder -= static_cast<double>(input.mi.dx);

			SendInput(1, &input, sizeof(input));
		}
	}

	void moveY(const double dy)
	{
		static double dyRemainder = 0.0;

		dyRemainder += dy;

		if (dyRemainder != 0.0)
		{
			INPUT input;

			ZeroMemory(&input, sizeof(input));

			input.type = INPUT_MOUSE;

			input.mi.dy = static_cast<LONG>(dyRemainder);
			input.mi.dwFlags = MOUSEEVENTF_MOVE;

			dyRemainder -= static_cast<double>(input.mi.dy);

			SendInput(1, &input, sizeof(input));
		}
	}

	void move(const double dx, const double dy)
	{
		static double dxRemainder = 0.0;
		static double dyRemainder = 0.0;
		
		dxRemainder += dx;
		dyRemainder -= dy;

		if (dxRemainder != 0.0 || dyRemainder != 0.0)
		{
			INPUT input;

			ZeroMemory(&input, sizeof(input));

			input.type = INPUT_MOUSE;

			input.mi.dx = static_cast<LONG>(dxRemainder);
			input.mi.dy = static_cast<LONG>(dyRemainder);
			input.mi.dwFlags = MOUSEEVENTF_MOVE;

			dxRemainder -= static_cast<double>(input.mi.dx);
			dyRemainder -= static_cast<double>(input.mi.dy);

			SendInput(1, &input, sizeof(input));
		}
	}

	void press(const Button::Name button)
	{
		static const DWORD dwFlagMap[] = {
			MOUSEEVENTF_LEFTDOWN,
			MOUSEEVENTF_MIDDLEDOWN,
			MOUSEEVENTF_RIGHTDOWN,
			MOUSEEVENTF_XDOWN,
			MOUSEEVENTF_XDOWN,
		};

		static const DWORD dwDataMap[] = {
			0,
			0,
			0,
			XBUTTON1,
			XBUTTON2,
		};

		if (button < Button::Count)
		{
			INPUT input;

			ZeroMemory(&input, sizeof(input));

			input.type = INPUT_MOUSE;

			input.mi.dwFlags = dwFlagMap[button];
			input.mi.mouseData = dwDataMap[button];

			SendInput(1, &input, sizeof(input));
		}
	}

	void release(const Button::Name button)
	{
		static const DWORD dwFlagMap[] = {
			MOUSEEVENTF_LEFTUP,
			MOUSEEVENTF_MIDDLEUP,
			MOUSEEVENTF_RIGHTUP,
			MOUSEEVENTF_XUP,
			MOUSEEVENTF_XUP,
		};

		static const DWORD dwDataMap[] = {
			0,
			0,
			0,
			XBUTTON1,
			XBUTTON2,
		};

		if (button < Button::Count)
		{
			INPUT input;

			ZeroMemory(&input, sizeof(input));

			input.type = INPUT_MOUSE;

			input.mi.dwFlags = dwFlagMap[button];
			input.mi.mouseData = dwDataMap[button];

			SendInput(1, &input, sizeof(input));
		}
	}

	void scrollX(const double dx)
	{
		static double dxRemainder = 0.0;

		dxRemainder += dx;

		if (dxRemainder != 0.0)
		{
			INPUT input;

			ZeroMemory(&input, sizeof(input));

			input.type = INPUT_MOUSE;

			input.mi.mouseData = static_cast<DWORD>(dxRemainder);
			input.mi.dwFlags = MOUSEEVENTF_HWHEEL;

			dxRemainder -= static_cast<double>(input.mi.mouseData);

			SendInput(1, &input, sizeof(input));
		}
	}

	void scrollY(const double dy)
	{
		static double dyRemainder = 0.0;

		dyRemainder += dy;

		if (dyRemainder != 0.0)
		{
			INPUT input;

			ZeroMemory(&input, sizeof(input));

			input.type = INPUT_MOUSE;

			input.mi.mouseData = static_cast<DWORD>(dyRemainder);
			input.mi.dwFlags = MOUSEEVENTF_WHEEL;

			dyRemainder -= static_cast<double>(input.mi.mouseData);

			SendInput(1, &input, sizeof(input));
		}
	}

	void scroll(const double dx, const double dy)
	{
		scrollX(dx);

		scrollY(dy);
	}

	void scrollStep(const Scroll::Name scroll)
	{
		switch (scroll)
		{
		case Scroll::Left:
		{
			scrollX(-static_cast<double>(WHEEL_DELTA));

			break;
		}

		case Scroll::Right:
		{
			scrollX(static_cast<double>(WHEEL_DELTA));

			break;
		}
		case Scroll::Up:
		{
			scrollY(static_cast<double>(WHEEL_DELTA));

			break;
		}

		case Scroll::Down:
		{
			scrollY(-static_cast<double>(WHEEL_DELTA));

			break;
		}
		default:
		{
			break;
		}
		}
	}
}
