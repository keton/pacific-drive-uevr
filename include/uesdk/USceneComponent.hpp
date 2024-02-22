#pragma once

#include "uevr/API.hpp"

using namespace uevr;

class USceneComponent : public API::UObject
{
  public:
	static API::UClass *static_class()
	{
		static auto result =
			API::get()->find_uobject<API::UClass>(L"Class /Script/Engine.SceneComponent");
		return result;
	}

	UEVR_Vector3f get_component_location()
	{
		static const auto func_candidate_1 =
			USceneComponent::static_class()->find_function(L"K2_GetComponentLocation");
		static const auto func_candidate_2 =
			USceneComponent::static_class()->find_function(L"GetComponentLocation");

		if(func_candidate_1 == nullptr && func_candidate_2 == nullptr) {
			return UEVR_Vector3f{0.0f, 0.0f, 0.0f};
		}

		auto func = func_candidate_1 != nullptr ? func_candidate_1 : func_candidate_2;

		struct
		{
			UEVR_Vector3f ret;
		} params{0};

		this->process_event(func, &params);

		return params.ret;
	}
};
