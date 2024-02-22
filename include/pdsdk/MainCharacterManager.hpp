#pragma once

#include "uevr/API.hpp"

using namespace uevr;

class MainCharacterManager : private API::UObject
{
  public:
	using API::UObject::get_full_name;

	enum ALS_MovementMode : uint8_t
	{
		None = 0,
		Grounded,
		Falling,
		Ragdoll,
		ForcedAnim,
		Car
	};

	static API::UClass *static_class()
	{
		static auto result = API::get()->find_uobject<API::UClass>(
			L"ManagerBlueprintGeneratedClass "
			L"/Game/Systems/Managers/BP_MainCharacterManager.BP_MainCharacterManager_C");
		return result;
	}

	static MainCharacterManager *get_instance()
	{
		auto instance = static_class()->get_first_object_matching<MainCharacterManager>();

		if(!instance) {
			API::get()->log_error("MainCharacterManager::get_instance() returned NULL");
		}

		return instance;
	}

	const ALS_MovementMode get_main_character_movement_mode()
	{
		const auto ret = get_property_data<ALS_MovementMode>(L"MainCharacterMovementModeRef");
		if(!ret) {
			API::get()->log_error(
				"get_main_character_movement_mode(): MainCharacterMovementModeRef NULL");
			return ALS_MovementMode::None;
		}

		return *ret;
	}
};
