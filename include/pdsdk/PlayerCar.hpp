#pragma once

#include <vector>

#include "uevr/API.hpp"

using namespace uevr;

class PlayerCar : private API::UObject
{
  public:
	using API::UObject::get_full_name;

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

	static API::UClass *static_class()
	{
		static auto result = API::get()->find_uobject<API::UClass>(
			L"BlueprintGeneratedClass "
			L"/Game/Characters/PlayerCar/BP_PlayerCarNew.BP_PlayerCarNew_C");
		return result;
	}

	static PlayerCar *get_instance()
	{
		auto instance = static_class()->get_first_object_matching<PlayerCar>();

		if(!instance) {
			API::get()->log_error("PlayerCar::get_instance() returned NULL");
		}

		return instance;
	}

	void toggle_gearbox_state(GearBoxState state, bool force_no_anim = false)
	{
		static const auto func = static_class()->find_function(L"ToggleGearBoxState");

		struct
		{
			uint8_t state;
			uint8_t force_no_anim;
		} params{.state = (uint8_t)state, .force_no_anim = force_no_anim};

		process_event(func, &params);
	}

	void toggle_ignition_state(IgnitionState state)
	{
		static const auto func = static_class()->find_function(L"ToggleIgnitionState");

		struct
		{
			uint8_t state;
		} params{.state = (uint8_t)state};

		process_event(func, &params);
	}

	void look_left_right(float axis_value)
	{
		static const auto func = static_class()->find_function(L"LookLeftRight");

		struct
		{
			float axis_value;
		} params{.axis_value = axis_value};

		process_event(func, &params);
	}

	void look_up_down(float axis_value)
	{
		static const auto func = static_class()->find_function(L"LookUpDown");

		struct
		{
			float axis_value;
		} params{.axis_value = axis_value};

		process_event(func, &params);
	}

	bool reset_camera()
	{
		static const auto func = static_class()->find_function(L"ResetCamera");

		struct
		{
			uint8_t reset_complete;
		} params{.reset_complete = 0};

		process_event(func, &params);

		return params.reset_complete;
	}
};
