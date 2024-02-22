#pragma once

#include "pdsdk/BaseHandManifestation.hpp"
#include "uevr/API.hpp"

using namespace uevr;

class MainCharacter : private API::UObject
{
  public:
	using API::UObject::get_full_name;

	static API::UClass *static_class()
	{
		static auto result = API::get()->find_uobject<API::UClass>(
			L"BlueprintGeneratedClass "
			L"/Game/Characters/Player/PDP_MainCharacter.PDP_MainCharacter_C");
		return result;
	}

	static MainCharacter *get_instance()
	{
		auto instance = static_class()->get_first_object_matching<MainCharacter>();

		if(!instance) {
			API::get()->log_error("MainCharacter::get_instance() returned NULL");
		}

		return instance;
	}

	float get_player_in_car_percent()
	{
		static const auto func = static_class()->find_function(L"GetPlayerInCarPercent");

		struct
		{
			float result;
		} params{.result = 0.0};

		process_event(func, &params);
		// API::get()->log_info("get_player_in_car_percent(): %f", params.result);

		return params.result;
	}

	bool is_player_in_car()
	{
		return get_player_in_car_percent() >= 1.0f; // actually can go above 100%
	}

	BaseHandManifestation *get_active_hand_item(bool ignore_lifespan = false)
	{
		static const auto func = static_class()->find_function(L"GetActiveHandItem");

		struct
		{
			uint8_t ignore_lifespan;
			BaseHandManifestation *out_hand_item;

			// values below are not get/set on call
			void *ignore_CallFunc_GetDrivingGameInstance_ReturnValue;
			void *ignore_CallFunc_FindManager_ReturnValue;
			void *ignore_K2Node_DynamicCast_AsBP_Main_Character_Manager;

			uint8_t ignore_K2Node_DynamicCast_bSuccess;

			void *ignore_CallFunc_GetActiveItem_Item;
			void *ignore_CallFunc_GetActiveItem_ItemManifestation;
			void *ignore_K2Node_DynamicCast_AsBP_Base_Hand_Manifestation;

			uint8_t ignore_K2Node_DynamicCast_bSuccess_1;

			float ignore_CallFunc_GetLifeSpan_ReturnValue;
			uint8_t ignore_CallFunc_EqualEqual_FloatFloat_ReturnValue;

		} params{0};

		params.ignore_lifespan = ignore_lifespan;

		process_event(func, &params);

		return params.out_hand_item;
	}
};
