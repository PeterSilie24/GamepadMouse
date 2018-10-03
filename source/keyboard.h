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

#include <Windows.h>
#include <shellapi.h>

extern void openOnScreenKeyboard();

extern void closeOnScreenKeyboard();

extern void toggleOnScreenKeyboard();

extern void keyboardPressKey(WORD wVk);

extern void keyboardReleaseKey(WORD wVk);

extern void keyboardPressReturn();

extern void keyboardReleaseReturn();

extern void keyboardPressControl();

extern void keyboardReleaseControl();

extern void keyboardPressUp();

extern void keyboardReleaseUp();

extern void keyboardPressDown();

extern void keyboardReleaseDown();

extern void keyboardPressLeft();

extern void keyboardReleaseLeft();

extern void keyboardPressRight();

extern void keyboardReleaseRight();

extern void keyboardShortcut(WORD wVk1, WORD wVk2);

extern void keyboardShortcutExtended(WORD wVk1, WORD wVk2, WORD wVk3);

extern void switchWindows();

extern void takeScreenshot();
