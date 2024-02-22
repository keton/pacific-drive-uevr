#pragma once

#include <vector>

#include "uevr/API.hpp"

using namespace uevr;

namespace PlayerCar {

enum GearBoxState
{
	Park = 0,
	Drive = 1
};
enum IgnitionState
{
	Off = 0,
	TurningOn,
	On
};
} // namespace PlayerCar

template <class T> class PlayerCarInterface : private API::UObject
{
  public:
	using API::UObject::get_full_name;

	static API::UClass *static_class()
	{
		return T::static_class();
	}

	void toggle_gearbox_state(PlayerCar::GearBoxState state, bool force_no_anim = false)
	{
		const auto s_class = static_class();
		if(!s_class) {
			return;
		}

		static const auto func = s_class->find_function(L"ToggleGearBoxState");
		if(!func) {
			API::get()->log_error("PlayerCarInterface::ToggleGearBoxState not found");
			return;
		}

		struct
		{
			uint8_t state;
			uint8_t force_no_anim;
		} params{.state = (uint8_t)state, .force_no_anim = force_no_anim};

		process_event(func, &params);
	}

	void toggle_ignition_state(PlayerCar::IgnitionState state)
	{
		const auto s_class = static_class();
		if(!s_class) {
			return;
		}

		static const auto func = s_class->find_function(L"ToggleIgnitionState");
		if(!func) {
			API::get()->log_error("PlayerCarInterface::ToggleIgnitionState not found");
			return;
		}

		struct
		{
			uint8_t state;
		} params{.state = (uint8_t)state};

		process_event(func, &params);
	}

	void look_left_right(float axis_value)
	{
		const auto s_class = static_class();
		if(!s_class) {
			return;
		}

		static const auto func = s_class->find_function(L"LookLeftRight");
		if(!func) {
			API::get()->log_error("PlayerCarInterface::LookLeftRight not found");
			return;
		}

		struct
		{
			float axis_value;
		} params{.axis_value = axis_value};

		process_event(func, &params);
	}

	void look_up_down(float axis_value)
	{
		const auto s_class = static_class();
		if(!s_class) {
			return;
		}

		static const auto func = s_class->find_function(L"LookUpDown");
		if(!func) {
			API::get()->log_error("PlayerCarInterface::LookUpDown not found");
			return;
		}

		struct
		{
			float axis_value;
		} params{.axis_value = axis_value};

		process_event(func, &params);
	}

	bool reset_camera()
	{
		const auto s_class = static_class();
		if(!s_class) {
			return false;
		}

		static const auto func = s_class->find_function(L"ResetCamera");
		if(!func) {
			API::get()->log_error("PlayerCarInterface::ResetCamera not found");
			return false;
		}

		struct
		{
			uint8_t reset_complete;
		} params{.reset_complete = 0};

		process_event(func, &params);

		return params.reset_complete;
	}
};

template <typename T>
concept PlayerCarConcept = std::is_base_of<PlayerCarInterface<T>, T>::value;
