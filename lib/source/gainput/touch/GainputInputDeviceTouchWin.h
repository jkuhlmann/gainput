// @author	Scott Mudge
// @file	This contains code extending Gainput to support native Windows multi-touch.

#ifndef GAINPUTINPUTDEVICETOUCHWINDOWS_H_
#define GAINPUTINPUTDEVICETOUCHWINDOWS_H_

#include "GainputInputDeviceTouchImpl.h"
#include "GainputTouchInfo.h"
#include "../GainputWindows.h"

#include <gainput/GainputInputDeltaState.h>
#include <gainput/GainputHelpers.h>
#include <gainput/GainputInputManager.h>

namespace gainput
{

	class InputDeviceTouchImplWin : public InputDeviceTouchImpl
	{
	public:
		InputDeviceTouchImplWin(InputManager& manager, InputDevice& device, InputState& state, InputState& previousState) :
			manager_(manager),
			device_(device),
			state_(&state),
			previousState_(&previousState),
			nextState_(manager.GetAllocator(), TouchCount_),
			delta_(0),
			touches_(manager.GetAllocator()),
			touchRegistered_(false),
			hwndBuff_(NULL),
			supportsPressure_(false)
		{
		}

		InputDevice::DeviceVariant GetVariant() const
		{
			return InputDevice::DV_STANDARD;
		}

		void Update(InputDeltaState* delta)
		{
			delta_ = delta;
			*state_ = nextState_;
		}

		InputDevice::DeviceState GetState() const { return InputDevice::DS_OK; }

		bool SupportsPressure() const
		{
			return supportsPressure_;
		}

		void SetSupportsPressure(bool supports)
		{
			supportsPressure_ = supports;
		}

		void HandleMessage(const MSG& msg)
		{
			GAINPUT_ASSERT(state_);
			GAINPUT_ASSERT(previousState_);

			RegisterTouch(msg.hwnd);

			DeviceButtonId buttonId;
			bool down = false;
			int ax = -1;
			int ay = -1;
			switch (msg.message)
			{
			case WM_TOUCH:
			{
				// Get number of touch inputs
				unsigned int num_inputs = (unsigned int)msg.wParam;
				// Create our struct array
				TOUCHINPUT* ti = new TOUCHINPUT[num_inputs];
				// Check
				if (!ti){
					GAINPUT_LOG("%s (%d): Could not create TOUCHINPUT array.", __FILE__, __LINE__);
					break;
				}

				// Populate array with touch data
				if (GetTouchInputInfo((HTOUCHINPUT)msg.lParam, num_inputs, ti, sizeof(TOUCHINPUT)) != TRUE) {
					GAINPUT_LOG("%s (%d): Could not get touch input info.", __FILE__, __LINE__);
					delete[] ti;
					break;
				}

				// Iterate
				for (unsigned int i = 0; i < num_inputs; ++i) {

					// Touch event -- Down
					if (ti[i].dwFlags & TOUCHEVENTF_DOWN) {
						float x = 0, y = 0;
						ConvertPtToClient(msg.hwnd, ti[i], x, y);
						
						// Test if ID is primary, if so, use TOUCH0
						const bool primary = (ti[i].dwFlags & TOUCHEVENTF_PRIMARY);

						const int touchIdx = GetTouchIDXFromID(primary, ti[i].dwID);

						if (touchIdx == TOUCH_IDX_NULL) {
							GAINPUT_LOG("%s (%d): Exceeded number of available touch inputs.", __FILE__, __LINE__);
							continue;
						}

						HandleBool(gainput::Touch0Down + touchIdx * TouchDataElems, true);
						HandleFloat(gainput::Touch0X + touchIdx * TouchDataElems, x);
						HandleFloat(gainput::Touch0Y + touchIdx * TouchDataElems, y);
						HandleFloat(gainput::Touch0Pressure + touchIdx * TouchDataElems, 0.0f); // Not Supported
					}

					// Touch event -- Move
					if (ti[i].dwFlags & TOUCHEVENTF_MOVE) {
						float x = 0, y = 0;
						ConvertPtToClient(msg.hwnd, ti[i], x, y);

						// Test if ID is primary, if so, use TOUCH0
						const bool primary = (ti[i].dwFlags & TOUCHEVENTF_PRIMARY);

						const int touchIdx = GetTouchIDXFromID(primary, ti[i].dwID);

						if (touchIdx == TOUCH_IDX_NULL) {
							GAINPUT_LOG("%s (%d): Exceeded number of available touch inputs.", __FILE__, __LINE__);
							continue;
						}

						HandleFloat(gainput::Touch0X + touchIdx * TouchDataElems, x);
						HandleFloat(gainput::Touch0Y + touchIdx * TouchDataElems, y);
						HandleFloat(gainput::Touch0Pressure + touchIdx * TouchDataElems, 0.0f); // Not Supported
					}

					// Touch event -- Up
					if (ti[i].dwFlags & TOUCHEVENTF_UP) {
						float x = 0, y = 0;
						ConvertPtToClient(msg.hwnd, ti[i], x, y);

						// Test if ID is primary, if so, use TOUCH0
						const bool primary = (ti[i].dwFlags & TOUCHEVENTF_PRIMARY);

						const int touchIdx = GetTouchIDXFromID(primary, ti[i].dwID);

						if (touchIdx == TOUCH_IDX_NULL) {
							GAINPUT_LOG("%s (%d): Exceeded number of available touch inputs.", __FILE__, __LINE__);
							continue;
						}

						HandleBool(gainput::Touch0Down + touchIdx * TouchDataElems, false);
						HandleFloat(gainput::Touch0X + touchIdx * TouchDataElems, x);
						HandleFloat(gainput::Touch0Y + touchIdx * TouchDataElems, y);
						HandleFloat(gainput::Touch0Pressure + touchIdx * TouchDataElems, 0.0f); // Not Supported

						FreeTouchIDFromIDXTable(primary, ti[i].dwID);
					}
				}

			}
			break;
			default: // Non-mouse message
				return;
			}
		}

	private:
		InputManager& manager_;
		InputDevice& device_;
		InputState* state_;
		InputState* previousState_;
		InputState nextState_;
		InputDeltaState* delta_;

		typedef gainput::Array< void* > TouchList;
		TouchList touches_;

		static const constexpr int TOUCH_IDX_NULL = INT_MAX-1;
		int touchIDs_[TouchCount_ / TouchDataElems];

		bool supportsPressure_;

		bool touchRegistered_;
		HWND hwndBuff_;

		void HandleBool(DeviceButtonId buttonId, bool value)
		{
			manager_.EnqueueConcurrentChange(device_, nextState_, delta_, buttonId, value);
		}

		void HandleFloat(DeviceButtonId buttonId, float value)
		{
			manager_.EnqueueConcurrentChange(device_, nextState_, delta_, buttonId, value);
		}

		/// Iterate through to retrieve ID from list. If it doesn't exist, grab the next one
		int GetTouchIDXFromID(const bool primary, const int id) {
			if (primary) return 0;
			// Try to find it in the list
			for (uint8_t i = 1; i < (TouchCount_ / TouchDataElems); ++i) {
				if (touchIDs_[i] == id) 
					return (int)i;
			}
			// Grab the next one.
			for (uint8_t i = 1; i < (TouchCount_ / TouchDataElems); ++i) {
				if (touchIDs_[i] == TOUCH_IDX_NULL) {
					touchIDs_[i] = id;
					return (int)i;
				}
			}
			// Too many in list already, return null index constant
			return TOUCH_IDX_NULL;
		}

		/// Frees a touch index from the IDX table. Returns true if the ID was found and freed.
		bool FreeTouchIDFromIDXTable(const bool primary, const int id) {
			if (primary) return true;
			// Try to find it in the list
			for (uint8_t i = 1; i < (TouchCount_ / TouchDataElems); ++i) {
				if (touchIDs_[i] == id) {
					touchIDs_[i] = TOUCH_IDX_NULL;
					return true;
				}
			}
			return false;
		}

		/// Converts supplied point to correct client region coordinates, accurate to hundredths of a pixel
		static void ConvertPtToClient(HWND hwnd, const TOUCHINPUT& ti, float& x, float& y) {
			// Get point
			POINT pt;
			pt.x = ti.x / 100;
			pt.y = ti.y / 100;

			const float
				frac_x = float(ti.x % 100) / 100.0f,
				frac_y = float(ti.y % 100) / 100.0f;

			ScreenToClient(hwnd, &pt);

			x = float(pt.x) + frac_x;
			y = float(pt.y) + frac_y;
		}

		/// Registers touch with supplied window handle
		void RegisterTouch(HWND hwnd) {
			// Check if window handle changed
			if (hwnd != hwndBuff_) {
				touchRegistered_ = false;
				hwndBuff_ = hwnd;
			}

			if (touchRegistered_)
				return;

			// Register window handle
			if (!RegisterTouchWindow(hwnd, 0)) {
				GAINPUT_LOG("%s (%d): Could not register touch window.", __FILE__, __LINE__);
				touchRegistered_ = false;
				return;
			}

			for (uint8_t i = 0; i < (TouchCount_ / TouchDataElems); ++i) 
				touchIDs_[i] = TOUCH_IDX_NULL;
			
			touchRegistered_ = true;
		}
	};

}


#endif //GAINPUTINPUTDEVICETOUCHWINDOWS_H_
