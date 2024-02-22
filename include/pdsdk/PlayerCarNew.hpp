#pragma once

#include <vector>

#include "uevr/API.hpp"

#include "PlayerCarInterface.hpp"

using namespace uevr;

class PlayerCarNew : public PlayerCarInterface<PlayerCarNew>
{
  public:
	using PlayerCarInterface::get_full_name;

	static API::UClass *static_class()
	{
		static auto result = API::get()->find_uobject<API::UClass>(
			L"BlueprintGeneratedClass "
			L"/Game/Characters/PlayerCar/BP_PlayerCarNew.BP_PlayerCarNew_C");

		return result;
	}

	static PlayerCarNew *get_instance()
	{
		const auto static_class = PlayerCarNew::static_class();
		if(!static_class) {
			return nullptr;
		}

		return static_class->get_first_object_matching<PlayerCarNew>();
	}
};
