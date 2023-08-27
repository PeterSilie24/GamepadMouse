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

namespace mouse
{
	extern void moveX(const double dx);
	
	extern void moveY(const double dy);

	extern void move(const double dx, const double dy);

	struct Button
	{
		typedef enum : unsigned int
		{
			Left,
			Middle,
			Right,
			X1,
			X2,
			Count
		} Name;
	};

	extern void press(const Button::Name button);

	extern void release(const Button::Name button);

	struct Scroll
	{
		typedef enum : unsigned int
		{
			Left,
			Right,
			Up,
			Down,
			Count
		} Name;
	};

	extern void scrollX(const double dx);

	extern void scrollY(const double dy);

	extern void scroll(const double dx, const double dy);

	extern void scrollStep(const Scroll::Name scroll);
}
