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

#include "gamepad.hpp"

#include <limits>
#include <cmath>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>

#undef min
#undef max

#pragma comment(lib, "XInput.lib")

namespace gp
{
	const XINPUT_STATE stateEmpty = {};

	void Button::update(const bool bPressed)
	{
		if (bPressed && !this->bPressed)
		{
			if (this->fPress)
			{
				this->fPress();
			}
		}
		else if (!bPressed && this->bPressed)
		{
			if (this->fRelease)
			{
				this->fRelease();
			}
		}

		this->bPressed = bPressed;
	}

	void Axis::update(const double dValue)
	{
		if (this->fCallback)
		{
			const double dDeadzoned = std::max(0.0, dValue - this->dThreshold) / (1.0 - this->dThreshold);

			this->fCallback(this->dSpeed * dDeadzoned);
		}
		else
		{
			const double dNormalized = (dValue - this->dReleaseThreshold) / (this->dPressThreshold - this->dReleaseThreshold);

			if (dNormalized >= 1.0 && !this->bPressed)
			{
				this->bPressed = true;

				if (this->fPress)
				{
					this->fPress();
				}
			}
			else if (dNormalized <= 0.0 && this->bPressed)
			{
				this->bPressed = false;

				if (this->fRelease)
				{
					this->fRelease();
				}
			}
		}
	}

	void Stick::update(const double dValueX, const double dValueY)
	{
		if (this->fCallback)
		{
			const double dLength = std::sqrt(dValueX * dValueX + dValueY * dValueY);
			const double dDeadzonedLength = std::max(0.0, dLength - this->dThreshold);

			if (dDeadzonedLength > 0.0)
			{
				const double dFactor = dDeadzonedLength / dLength / (1.0 - this->dThreshold);
				const double dDeadzonedX = dValueX * dFactor;
				const double dDeadzonedY = dValueY * dFactor;

				this->fCallback(this->dSpeed * dDeadzonedX, this->dSpeed * dDeadzonedY);
			}
		}
	}

	Gamepad::Gamepad(const int iIndex, const bool bEnabled) :
		iIndex(iIndex), bEnabled(bEnabled), tLast(std::chrono::steady_clock::now())
	{
		this->vButtons[false].resize(Button::Count);
		this->vButtons[true].resize(Button::Count);

		this->vAxes[false].resize(Axis::Count);
		this->vAxes[true].resize(Axis::Count);

		this->vSticks[false].resize(Stick::Count);
		this->vSticks[true].resize(Stick::Count);
	}

	Gamepad::~Gamepad()
	{
		this->disable();
	}

	void Gamepad::update()
	{
		std::chrono::steady_clock::time_point tNow = std::chrono::steady_clock::now();

		if (!this->bConnected && std::chrono::duration_cast<std::chrono::milliseconds>(tNow - this->tLast).count() < 250)
		{
			return;
		}

		this->tLast = tNow;

		XINPUT_STATE state;

		const XINPUT_STATE* states[2] = {
			&state,
			&stateEmpty
		};

		if (XInputGetState(static_cast<DWORD>(this->iIndex), &state) == ERROR_DEVICE_NOT_CONNECTED)
		{
			if (!this->bConnected)
			{
				return;
			}

			this->bConnected = false;

			states[0] = states[1];
		}
		else
		{
			this->bConnected = true;
		}

		if (this->bEnabled)
		{
			states[1] = states[0];
		}

		const WORD wButtonMaskMap[] = {
			XINPUT_GAMEPAD_DPAD_UP,
			XINPUT_GAMEPAD_DPAD_DOWN,
			XINPUT_GAMEPAD_DPAD_LEFT,
			XINPUT_GAMEPAD_DPAD_RIGHT,
			XINPUT_GAMEPAD_START,
			XINPUT_GAMEPAD_BACK,
			XINPUT_GAMEPAD_LEFT_THUMB,
			XINPUT_GAMEPAD_RIGHT_THUMB,
			XINPUT_GAMEPAD_LEFT_SHOULDER,
			XINPUT_GAMEPAD_RIGHT_SHOULDER,
			XINPUT_GAMEPAD_A,
			XINPUT_GAMEPAD_B,
			XINPUT_GAMEPAD_X,
			XINPUT_GAMEPAD_Y
		};

		for (int i = false; i <= (this->bEnabled ? 1 : 0); i++)
		{
			for (int iButton = 0; iButton < Button::Count; iButton++)
			{
				const bool bPressed = static_cast<bool>(states[i]->Gamepad.wButtons & wButtonMaskMap[iButton]);

				for (Button& button : this->vButtons[i][iButton])
				{
					button.update(bPressed);
				}
			}
		}

		auto fConversionCreator = []<typename T>(const T& tValue) -> std::function<double()>
		{
			if (std::is_signed_v<T>)
			{
				return [&]()
				{
					return 2.0 * (static_cast<double>(tValue) - static_cast<double>(std::numeric_limits<T>::min())) / (static_cast<double>(std::numeric_limits<T>::max()) - static_cast<double>(std::numeric_limits<T>::min())) - 1.0;
				};
			}
			else
			{
				return [&]()
				{
					return (static_cast<double>(tValue) - static_cast<double>(std::numeric_limits<T>::min())) / (static_cast<double>(std::numeric_limits<T>::max()) - static_cast<double>(std::numeric_limits<T>::min()));
				};
			}
		};

		const std::function<double()> fAxisConversionMap[2][6] = {
			{
				fConversionCreator(states[false]->Gamepad.bLeftTrigger),
				fConversionCreator(states[false]->Gamepad.bRightTrigger),
				fConversionCreator(states[false]->Gamepad.sThumbLX),
				fConversionCreator(states[false]->Gamepad.sThumbLY),
				fConversionCreator(states[false]->Gamepad.sThumbRX),
				fConversionCreator(states[false]->Gamepad.sThumbRY)
			},
			{
				fConversionCreator(states[true]->Gamepad.bLeftTrigger),
				fConversionCreator(states[true]->Gamepad.bRightTrigger),
				fConversionCreator(states[true]->Gamepad.sThumbLX),
				fConversionCreator(states[true]->Gamepad.sThumbLY),
				fConversionCreator(states[true]->Gamepad.sThumbRX),
				fConversionCreator(states[true]->Gamepad.sThumbRY)
			}
		};

		for (int i = false; i <= (this->bEnabled ? 1 : 0); i++)
		{
			for (int iAxis = 0; iAxis < Axis::Count; iAxis++)
			{
				const double dValue = fAxisConversionMap[i][iAxis]();

				for (Axis& axis : this->vAxes[i][iAxis])
				{
					axis.update(dValue);
				}
			}
		}

		for (int i = false; i <= (this->bEnabled ? 1 : 0); i++)
		{
			for (int iStick = 0; iStick < Stick::Count; iStick++)
			{
				const double dValueX = iStick == Stick::Left ? fAxisConversionMap[i][Axis::StickLeftX]() : fAxisConversionMap[i][Axis::StickRightX]();
				const double dValueY = iStick == Stick::Left ? fAxisConversionMap[i][Axis::StickLeftY]() : fAxisConversionMap[i][Axis::StickRightY]();

				for (Stick& stick : this->vSticks[i][iStick])
				{
					stick.update(dValueX, dValueY);
				}
			}
		}
	}

	bool Gamepad::isConnected() const
	{
		return this->bConnected;
	}

	bool Gamepad::isEnabled() const
	{
		return this->bEnabled;
	}

	void Gamepad::enable(const bool bEnable)
	{
		this->bEnabled = bEnable;
	}

	void Gamepad::disable()
	{
		this->enable(false);
	}

	void Gamepad::toggle()
	{
		this->enable(!this->bEnabled);
	}

	bool Gamepad::isReady() const
	{
		return this->isConnected() && this->isEnabled();
	}
	
	GamepadPtr make(const int iIndex, const bool bEnabled)
	{
		return std::make_shared<Gamepad>(iIndex, bEnabled);
	}

	GamepadPtr makeDefault(const int iIndex, const bool bEnabled)
	{
		GamepadPtr gamepad = make(iIndex, bEnabled);

		gamepad->button(Button::A, mouse::Button::Left);
		gamepad->button(Button::B, mouse::Button::Right);
		gamepad->button(Button::X, key::Key::Return);
		gamepad->button(Button::Y, key::onScreenKeyboardToggle);
		gamepad->button(Button::DpadUp, key::Key::Up);
		gamepad->button(Button::DpadDown, key::Key::Down);
		gamepad->button(Button::DpadLeft, key::Key::Left);
		gamepad->button(Button::DpadRight, key::Key::Right);
		gamepad->button(Button::ThumbLeft, mouse::Button::Middle);
		gamepad->button(Button::ThumbRight, key::Key::Control);
		gamepad->button(Button::ShoulderLeft, key::switchWindows);
		gamepad->button(Button::ShoulderRight, key::takeScreenshot);

		gamepad->axis(Axis::TriggerLeft, mouse::scrollY, 10.0);
		gamepad->axis(Axis::TriggerRight, mouse::scrollY, -10.0);

		gamepad->stick(Stick::Left, mouse::move, 10.0);
		gamepad->stick(Stick::Right, mouse::scroll, 10.0);

		gamepad->combination<true>(Button::Back, Button::Start, Gamepad::Event::Toggle);

		return gamepad;
	}
}
