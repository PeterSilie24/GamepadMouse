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

#include <cstddef>
#include <functional>
#include <vector>
#include <chrono>
#include <memory>

#include "mouse.hpp"
#include "keyboard.hpp"

namespace gp
{
	class Gamepad;

	class Button
	{
	public:
		friend class Gamepad;

		typedef enum : unsigned int
		{
			DpadUp,
			DpadDown,
			DpadLeft,
			DpadRight,
			Start,
			Back,
			ThumbLeft,
			ThumbRight,
			ShoulderLeft,
			ShoulderRight,
			A,
			B,
			X,
			Y,
			Count
		} Name;

	private:
		bool bPressed = false;

		std::function<void()> fPress = nullptr;
		std::function<void()> fRelease = nullptr;

		template <typename FPress, typename FRelease>
		Button(FPress fPress, FRelease fRelease) :
			fPress(fPress), fRelease(fRelease)
		{

		}

		void update(const bool bPressed);
	};

	class Axis
	{
	public:
		friend class Gamepad;

		typedef enum : unsigned int
		{
			TriggerLeft,
			TriggerRight,
			StickLeftX,
			StickLeftY,
			StickRightX,
			StickRightY,
			Count
		} Name;

	private:
		bool bPressed = false;

		union
		{
			struct
			{
				double dPressThreshold;
				double dReleaseThreshold;
			};

			struct
			{
				double dSpeed;
				double dThreshold;
			};
		};

		std::function<void()> fPress = nullptr;
		std::function<void()> fRelease = nullptr;

		std::function<void(const double)> fCallback = nullptr;
		

		template <typename FCallback>
		Axis(FCallback fCallback, const double dSpeed, const double dThreshold) :
			fCallback(fCallback), dSpeed(dSpeed), dThreshold(dThreshold)
		{

		}

		template <typename FPress, typename FRelease>
		Axis(FPress fPress, FRelease fRelease, const double dPressThreshold, const double dReleaseThreshold) :
			fPress(fPress), fRelease(fRelease), dPressThreshold(dPressThreshold), dReleaseThreshold(dReleaseThreshold)
		{

		}

		void update(const double dValue);
	};

	class Stick
	{
	public:
		friend class Gamepad;

		typedef enum : int
		{
			Left,
			Right,
			Count
		} Name;

	private:
		std::function<void(const double, const double)> fCallback = nullptr;

		double dSpeed = 1.0;
		double dThreshold = 0.0;

		template <typename FCallback>
		Stick(FCallback fCallback, const double dSpeed, const double dThreshold) :
			fCallback(fCallback), dSpeed(dSpeed), dThreshold(dThreshold)
		{

		}

		void update(const double dValueX, const double dValueY);
	};

	class Gamepad
	{
	public:
		struct Event
		{
			typedef enum : int
			{
				Enable,
				Disable,
				Toggle,
				Count
			} Name;
		};

	private:
		int iIndex = 0;

		bool bConnected = false;
		bool bEnabled = true;

		std::vector<std::vector<Button>> vButtons[2];
		std::vector<std::vector<Axis>> vAxes[2];
		std::vector<std::vector<Stick>> vSticks[2];

		std::chrono::steady_clock::time_point tLast;

	public:
		Gamepad(const int iIndex = 0, const bool bEnabled = true);

		~Gamepad();

		Gamepad(const Gamepad&) = delete;
		Gamepad(Gamepad&&) = delete;

		Gamepad& operator=(const Gamepad&) = delete;
		Gamepad& operator=(Gamepad&&) = delete;

		void update();

		template <const bool alwaysEnabled = false, typename FPress = std::function<void()>, typename FRelease = std::function<void()>>
		requires(std::is_constructible_v<std::function<void()>, FPress> && std::is_constructible_v<std::function<void()>, FRelease>)
		void button(const Button::Name button, FPress fPress = [] {}, FRelease fRelease = [] {})
		{
			if (button < Button::Count)
			{
				this->vButtons[!alwaysEnabled][button].push_back(Button(fPress, fRelease));
			}
		}

		template <const bool alwaysEnabled = false>
		void button(const Button::Name button, const mouse::Button::Name mouseButton)
		{
			this->button<alwaysEnabled>(button, [mouseButton]() { mouse::press(mouseButton); }, [mouseButton]() { mouse::release(mouseButton); });
		}

		template <const bool alwaysEnabled = false>
		void button(const Button::Name button, const mouse::Scroll::Name mouseScroll)
		{
			this->button<alwaysEnabled>(button, [mouseScroll]() { mouse::scrollStep(mouseScroll); }, [] {});
		}

		template <const bool alwaysEnabled = false>
		void button(const Button::Name button, const key::Key::Name key)
		{
			this->button<alwaysEnabled>(button, [key]() { key::press(key); }, [key]() { key::release(key); });
		}

		template <const bool alwaysEnabled = false>
		void button(const Button::Name button, const Event::Name event)
		{
			switch (event)
			{
			case Event::Enable:
			{
				return this->button<alwaysEnabled>(button, [this]() { this->enable(); }, [] { });
			}
			case Event::Disable:
			{
				return this->button<alwaysEnabled>(button, [this]() { this->disable(); }, [] {});
			}
			case Event::Toggle:
			{
				return this->button<alwaysEnabled>(button, [this]() { this->toggle(); }, [] {});
			}
			default:
			{
				break;
			}
			}
		}

		template <const bool alwaysEnabled = false, typename FCallback = std::function<void()>>
		requires(std::is_constructible_v<std::function<void(const double)>, FCallback>)
		void axis(const Axis::Name axis, FCallback fCallback = [](const double) {}, const double dSpeed = 1.0, const double dThreshold = 0.25)
		{
			if (axis < Axis::Count)
			{
				this->vAxes[!alwaysEnabled][axis].push_back(Axis(fCallback, dSpeed, dThreshold));
			}
		}

		template <const bool alwaysEnabled = false, typename FPress = std::function<void()>, typename FRelease = std::function<void()>>
		requires(std::is_constructible_v<std::function<void()>, FPress> && std::is_constructible_v<std::function<void()>, FRelease>)
		void axisButton(const Axis::Name axis, FPress fPress = [] {}, FRelease fRelease = [] {}, const double dPressThreshold = 0.5, const double dReleaseThreshold = 0.25)
		{
			if (axis < Axis::Count)
			{
				this->vAxes[!alwaysEnabled][axis].push_back(Axis(fPress, fRelease, dPressThreshold, dReleaseThreshold));
			}
		}

		template <const bool alwaysEnabled = false>
		void axisButton(const Axis::Name axis, const mouse::Button::Name mouseButton, const double dPressThreshold = 0.5, const double dReleaseThreshold = 0.25)
		{
			this->axisButton<alwaysEnabled>(axis, [mouseButton]() { mouse::press(mouseButton); }, [mouseButton]() { mouse::release(mouseButton); }, dPressThreshold, dReleaseThreshold);
		}

		template <const bool alwaysEnabled = false>
		void axisButton(const Axis::Name axis, const mouse::Scroll::Name mouseScroll, const double dPressThreshold = 0.5, const double dReleaseThreshold = 0.25)
		{
			this->axisButton<alwaysEnabled>(axis, [mouseScroll]() { mouse::scrollStep(mouseScroll); }, [] {}, dPressThreshold, dReleaseThreshold);
		}

		template <const bool alwaysEnabled = false>
		void axisButton(const Axis::Name axis, const key::Key::Name key, const double dPressThreshold = 0.5, const double dReleaseThreshold = 0.25)
		{
			this->axisButton<alwaysEnabled>(axis, [key]() { key::press(key); }, [key]() { key::release(key); }, dPressThreshold, dReleaseThreshold);
		}

		template <const bool alwaysEnabled = false>
		void axisButton(const Axis::Name axis, const Event::Name event, const double dPressThreshold = 0.5, const double dReleaseThreshold = 0.25)
		{
			switch (event)
			{
			case Event::Enable:
			{
				return this->axisButton<alwaysEnabled>(button, [this]() { this->enable(); }, [] {}, dPressThreshold, dReleaseThreshold);
			}
			case Event::Disable:
			{
				return this->axisButton<alwaysEnabled>(button, [this]() { this->disable(); }, [] {}, dPressThreshold, dReleaseThreshold);
			}
			case Event::Toggle:
			{
				return this->axisButton<alwaysEnabled>(button, [this]() { this->toggle(); }, [] {}, dPressThreshold, dReleaseThreshold);
			}
			default:
			{
				break;
			}
			}
		}

		template <const bool alwaysEnabled = false, typename FCallback = std::function<void()>>
		requires(std::is_constructible_v<std::function<void(const double, const double)>, FCallback>)
		void stick(const Stick::Name stick, FCallback fCallback = [](const double, const double) {}, const double dSpeed = 1.0, const double dThreshold = 0.25)
		{
			if (stick < Stick::Count)
			{
				this->vSticks[!alwaysEnabled][stick].push_back(Stick(fCallback, dSpeed, dThreshold));
			}
		}

	private:
		struct Combination
		{
			std::function<void()> fPress = nullptr;
			std::function<void()> fRelease = nullptr;

			std::size_t szCounter = 0;
			std::size_t szCount = 0;
		};

		template <const bool alwaysEnabled = false, typename... Arguments>
		void combinationSelector(const std::shared_ptr<Combination>& pCombination, const Button::Name button, Arguments&&... arguments)
		{
			this->combinationButton<alwaysEnabled>(pCombination, button, std::forward<Arguments>(arguments)...);
		}

		template <const bool alwaysEnabled = false, typename... Arguments>
		void combinationButton(const std::shared_ptr<Combination>& pCombination, const Button::Name button, Arguments&&... arguments)
		{
			pCombination->szCount++;

			auto fPress = [pCombination]()
			{
				pCombination->szCounter++;

				if (pCombination->szCounter == pCombination->szCount)
				{
					pCombination->fPress();
				}
			};

			auto fRelease = [pCombination]()
			{
				if (pCombination->szCounter == pCombination->szCount)
				{
					pCombination->fRelease();
				}

				pCombination->szCounter--;
			};

			this->button<alwaysEnabled>(button, fPress, fRelease);

			this->combinationSelector<alwaysEnabled>(pCombination, std::forward<Arguments>(arguments)...);
		}

		template <const bool alwaysEnabled = false, typename... Arguments>
		void combinationSelector(const std::shared_ptr<Combination>& pCombination, const Axis::Name axis, Arguments&&... arguments)
		{
			this->combinationAxisButton<alwaysEnabled>(pCombination, axis, std::forward<Arguments>(arguments)...);
		}

		template <const bool alwaysEnabled = false, typename... Arguments>
		void combinationAxisButton(const std::shared_ptr<Combination>& pCombination, const Axis::Name axis, const double dPressThreshold, Arguments&&... arguments)
		{
			this->combinationAxisButtonPressThreshold<alwaysEnabled>(pCombination, axis, dPressThreshold, std::forward<Arguments>(arguments)...);
		}

		template <const bool alwaysEnabled = false, typename Argument, typename... Arguments>
		requires(!std::is_same_v<Argument, double>)
		void combinationAxisButton(const std::shared_ptr<Combination>& pCombination, const Axis::Name axis, Argument&& argument, Arguments&&... arguments)
		{
			this->combinationAxisButtonPressThreshold<alwaysEnabled>(pCombination, axis, 0.5, std::forward<Argument>(argument), std::forward<Arguments>(arguments)...);
		}

		template <const bool alwaysEnabled = false, typename... Arguments>
		void combinationAxisButtonPressThreshold(const std::shared_ptr<Combination>& pCombination, const Axis::Name axis, const double dPressThreshold, const double dReleaseThreshold, Arguments&&... arguments)
		{
			this->combinationAxisButtonPressThresholdReleaseThreshold<alwaysEnabled>(pCombination, axis, dPressThreshold, dReleaseThreshold, std::forward<Arguments>(arguments)...);
		}

		template <const bool alwaysEnabled = false, typename Argument, typename... Arguments>
		requires(!std::is_same_v<Argument, double>)
		void combinationAxisButtonPressThreshold(const std::shared_ptr<Combination>& pCombination, const Axis::Name axis, const double dPressThreshold, Argument&& argument, Arguments&&... arguments)
		{
			this->combinationAxisButtonPressThresholdReleaseThreshold<alwaysEnabled>(pCombination, axis, dPressThreshold, 0.25, std::forward<Argument>(argument), std::forward<Arguments>(arguments)...);
		}

		template <const bool alwaysEnabled = false, typename... Arguments>
		void combinationAxisButtonPressThresholdReleaseThreshold(const std::shared_ptr<Combination>& pCombination, const Axis::Name axis, const double dPressThreshold, const double dReleaseThreshold, Arguments&&... arguments)
		{
			pCombination->szCount++;

			auto fPress = [pCombination]()
			{
				pCombination->szCounter++;

				if (pCombination->szCounter == pCombination->szCount)
				{
					pCombination->fPress();
				}
			};

			auto fRelease = [pCombination]()
			{
				if (pCombination->szCounter == pCombination->szCount)
				{
					pCombination->fRelease();
				}

				pCombination->szCounter--;
			};

			this->axisButton<alwaysEnabled>(axis, fPress, fRelease, dPressThreshold, dReleaseThreshold);

			this->combinationSelector<alwaysEnabled>(pCombination, std::forward<Arguments>(arguments)...);
		}

		template <const bool alwaysEnabled = false, typename Argument, typename... Arguments>
		requires(!std::is_same_v<Argument, Button::Name> && !std::is_same_v<Argument, Axis::Name>)
		void combinationSelector(const std::shared_ptr<Combination>& pCombination, Argument&& argument, Arguments&&... arguments)
		{
			this->combinationCreate(pCombination, std::forward<Argument>(argument), std::forward<Arguments>(arguments)...);
		}

		template <typename FPress = std::function<void()>, typename FRelease = std::function<void()>>
		void combinationCreate(const std::shared_ptr<Combination>& pCombination, FPress fPress = [] {}, FRelease fRelease = [] {})
		{
			pCombination->fPress = fPress;
			pCombination->fRelease = fRelease;
		}

		void combinationCreate(const std::shared_ptr<Combination>& pCombination, const mouse::Button::Name mouseButton)
		{
			this->combinationCreate(pCombination, [mouseButton]() { mouse::press(mouseButton); }, [mouseButton]() { mouse::release(mouseButton); });
		}

		void combinationCreate(const std::shared_ptr<Combination>& pCombination, const mouse::Scroll::Name mouseScroll)
		{
			this->combinationCreate(pCombination, [mouseScroll]() { mouse::scrollStep(mouseScroll); }, [] {});
		}

		void combinationCreate(const std::shared_ptr<Combination>& pCombination, const key::Key::Name key)
		{
			this->combinationCreate(pCombination, [key]() { key::press(key); }, [key]() { key::release(key); });
		}

		void combinationCreate(const std::shared_ptr<Combination>& pCombination, const Event::Name event)
		{
			if (event < Event::Count)
			{
				switch (event)
				{
				case Event::Enable:
				{
					return this->combinationCreate(pCombination, [this]() { this->enable(); }, [] {});
				}
				case Event::Disable:
				{
					return this->combinationCreate(pCombination, [this]() { this->disable(); }, [] {});
				}
				case Event::Toggle:
				{
					return this->combinationCreate(pCombination, [this]() { this->toggle(); }, [] {});
				}
				default:
				{
					break;
				}
				}
			}
		}

	public:
		template <const bool alwaysEnabled = false, typename... Arguments>
		void combination(Arguments&&... arguments)
		{
			this->combinationSelector<alwaysEnabled>(std::make_shared<Combination>(), std::forward<Arguments>(arguments)...);
		}

		bool isConnected() const;

		bool isEnabled() const;

		void enable(const bool bEnable = true);

		void disable();

		void toggle();

		bool isReady() const;
	};

	typedef std::shared_ptr<Gamepad> GamepadPtr;

	extern GamepadPtr make(const int iIndex = 0, const bool bEnabled = true);

	extern GamepadPtr makeDefault(const int iIndex = 0, const bool bEnabled = true);
}
