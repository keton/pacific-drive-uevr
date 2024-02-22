#pragma once

#include <vector>

#include "uevr/API.hpp"

#include "PlayerCarInterface.hpp"

using namespace uevr;

class PlayerCarIntro : public PlayerCarInterface<PlayerCarIntro>
{
  public:
	using PlayerCarInterface::get_full_name;

	// BP_PlayerCar_Intro_C does not exist in normal game object tree, we have to keep looking
	static inline API::UClass *static_class_inner()
	{
		return API::get()->find_uobject<API::UClass>(
			L"BlueprintGeneratedClass "
			L"/Game/Characters/PlayerCar/BP_PlayerCar_Intro.BP_PlayerCar_Intro_C");
	}

	static API::UClass *static_class(bool redo_search = false)
	{
		static auto result = static_class_inner();

		if(!result && redo_search) {
			result = static_class_inner();
		}

		return result;
	}

	static PlayerCarIntro *get_instance(bool redo_search = false)
	{
		const auto static_class = PlayerCarIntro::static_class(redo_search);
		if(!static_class) {
			return nullptr;
		}

		return PlayerCarIntro::static_class()->get_first_object_matching<PlayerCarIntro>();
	}
};
