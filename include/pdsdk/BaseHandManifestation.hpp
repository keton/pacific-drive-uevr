#pragma once

#include <map>

#include "uevr/API.hpp"

#include "uesdk/AActor.hpp"

using namespace uevr;

class BaseHandManifestation : public AActor
{
  public:
	enum Type
	{
		Unknown,

		GarageGasPumpHandle,
		Decal,
		Paint,
		AnchorRadar,
		BatteryCharger,
		Battery,
		BioLantern,
		Breaker,
		Cutter,
		Extractor,
		FixBulb,
		FixElectronic,
		FixMechanical,
		FixSeal,
		ThrowFlare,
		FlareGun,
		BaseFlashLight,
		CampingLight,
		Flashlight,
		HoldFood,
		FuelCan,
		HealerBasic,
		HoldMedicine,
		HoldMedPack,
		PatchKit,
		Scrapper,
		Unlocker,
		Vacuum,
		ResourceGasPumpHandle,
	};

	static std::string type_to_str(BaseHandManifestation::Type type);

	static API::UClass *static_class()
	{
		static auto result = API::get()->find_uobject<API::UClass>(
			L"BlueprintGeneratedClass "
			L"/Game/Gameplay/Inventory/BaseItems/Hand/"
			L"BP_BaseHandManifestation.BP_BaseHandManifestation_C");
		return result;
	}

	static std::vector<BaseHandManifestation *> get_instances()
	{
		auto instances = static_class()->get_objects_matching<BaseHandManifestation>();

		if(instances.size() == 0) {
			API::get()->log_error("BaseHandManifestation::get_instances() empty");
		}

		return instances;
	}

	const Type get_type();

	const UEVR_Vector3f get_scale();
	const UEVR_Quaternionf get_rotation_offset();
	const UEVR_Vector3f get_position_offset();
};
