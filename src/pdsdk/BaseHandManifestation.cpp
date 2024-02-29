#include <map>

#include "Math.hpp"
#include "pdsdk/BaseHandManifestation.hpp"

using bmt = BaseHandManifestation::Type;

static const std::map<std::wstring, BaseHandManifestation::Type> name_prefix_to_item_type = {
	// clang-format off

    {L"BP_Tool_Target_Garage_GasPumpHandle_", bmt::GarageGasPumpHandle},
    {L"BP_Tool_Target_Decal_", bmt::Decal},
    {L"BP_Tool_Target_Paint_", bmt::Paint},
    {L"BP_Tool_AnchorRadar_", bmt::AnchorRadar},
    {L"BP_Tool_Target_BatteryCharger_", bmt::BatteryCharger},
    {L"BP_Tool_Target_Consumable_ChargeBattery_", bmt::Battery},
    {L"BP_Tool_Toggle_Biolantern_", bmt::BioLantern},
    {L"BP_Tool_Target_Breaker_", bmt::Breaker},
    {L"BP_Tool_Target_ItemHarvest_Cutter_", bmt::Cutter},
    {L"BP_Tool_Target_Extractor_", bmt::Extractor},
    {L"BP_Tool_Target_FixBulb_", bmt::FixBulb},
    {L"BP_Tool_Target_FixElectronic_", bmt::FixElectronic},
    {L"BP_Tool_Target_FixMechanical_", bmt::FixMechanical},
    {L"BP_Tool_Target_FixSeal_", bmt::FixSeal},
    {L"BP_Tool_Throw_Flare_Timers_", bmt::ThrowFlare},
    {L"BP_Tool_Shoot_Flare_Gun_", bmt::FlareGun},
    {L"BP_Tool_Toggle_BaseFlashLight_", bmt::BaseFlashLight},
    {L"BP_Tool_Toggle_CampingLight_", bmt::CampingLight},
    {L"BP_Tool_Toggle_Flashlight_", bmt::Flashlight},
    {L"BP_Tool_Hold_Food_", bmt::HoldFood},
    {L"BP_Tool_Target_FuelCan_", bmt::FuelCan},
    {L"BP_Tool_Target_Healer_Basic_", bmt::HealerBasic},
    {L"BP_Tool_Hold_Medicine_", bmt::HoldMedicine},
    {L"BP_Tool_Hold_MedPack_", bmt::HoldMedPack},
    {L"BP_Tool_Target_Consumable_PatchKit_", bmt::PatchKit},
    {L"BP_Tool_Target_ItemHarvest_Scrapper_", bmt::Scrapper},
    {L"BP_Tool_Target_Unlocker_", bmt::Unlocker},
    {L"BP_Tool_Target_ItemHarvest_Vacuum_", bmt::Vacuum},
    {L"BP_Tool_Target_Resource_GasPumpHandle_", bmt::ResourceGasPumpHandle},

	// clang-format on
};

// item type to scale that should be applied on attach
static const std::map<BaseHandManifestation::Type, UEVR_Vector3f> item_type_to_scale{
	// clang-format off

	{bmt::Unknown, {0.7f, 0.7f, 0.7f}}, // default scale if not found

	{bmt::Breaker, {0.5f, 0.5f, 0.5f}},
	{bmt::FuelCan, {1.2f, 1.2f, 1.2f}},
	{bmt::Scrapper, {0.45f, 0.45f, 0.45f}},
	{bmt::Unlocker, {0.7f, 0.7f, 0.7f}},
	{bmt::Vacuum, {0.5f, 0.5f, 0.5f}},

	// clang-format on
};

// item type to rotation that should be applied on attach
static const std::map<BaseHandManifestation::Type, UEVR_Vector3f> item_type_to_rotation_offset{
	{bmt::Unknown, {.x = 0.0f, .y = 0.0f, .z = 0.0f}}, // default rotation if not found

	{bmt::Flashlight, {1.486f, 0.771f, 1.036f}},
	{bmt::GarageGasPumpHandle, {.x = -0.016f, .y = 1.607f, .z = 1.593f}},
	{bmt::ResourceGasPumpHandle, {.x = 0.057f, .y = 1.622f, .z = 1.685f}},
	{bmt::Unlocker, {.x = 0.007f, .y = 0.081f, .z = -3.129f}},
};

// item type to position offset that should be applied on attach
static const std::map<BaseHandManifestation::Type, UEVR_Vector3f> item_type_to_position_offset{
	{bmt::Unknown, {0.0f, 0.0f, 0.0f}}, // default position if not found

	{bmt::Flashlight, {0.647f, -5.304f, 0.693f}},
	{bmt::GarageGasPumpHandle, {-18.852f, -4.755f, -18.571f}},
	{bmt::ResourceGasPumpHandle, {-7.775f, 6.137f, -7.347f}},
	{bmt::Unlocker, {3.587f, 44.140f, 18.766f}},
};

const UEVR_Vector3f BaseHandManifestation::get_scale()
{
	const auto type = get_type();

	UEVR_Vector3f ret{1.0f, 1.0f, 1.0f};

	const auto it = item_type_to_scale.find(type);
	if(it != item_type_to_scale.end()) {
		ret = it->second;
	} else {
		ret = item_type_to_scale.at(Unknown);
	}

	return ret;
}

const UEVR_Quaternionf BaseHandManifestation::get_rotation_offset()
{
	const auto type = get_type();

	UEVR_Vector3f rotation{};

	const auto it = item_type_to_rotation_offset.find(type);
	if(it != item_type_to_rotation_offset.end()) {
		rotation = it->second;
	} else {
		rotation = item_type_to_rotation_offset.at(Unknown);
	}

	const auto rotation_glm_mat = glm::yawPitchRoll(-rotation.y, rotation.x, -rotation.z);
	const auto rotation_glm_quat = glm::quat{rotation_glm_mat};

	return {
		.w = rotation_glm_quat.w,
		.x = rotation_glm_quat.x,
		.y = rotation_glm_quat.y,
		.z = rotation_glm_quat.z,
	};
}

const UEVR_Vector3f BaseHandManifestation::get_position_offset()
{
	const auto type = get_type();

	UEVR_Vector3f ret{};

	const auto it = item_type_to_position_offset.find(type);
	if(it != item_type_to_position_offset.end()) {
		ret = it->second;
	} else {
		ret = item_type_to_position_offset.at(Unknown);
	}

	return ret;
}

const BaseHandManifestation::Type BaseHandManifestation::get_type()
{
	const auto name = get_full_name();

	for(const auto &[class_name_prefix, class_type] : name_prefix_to_item_type) {

		if(name.starts_with(class_name_prefix)) {
			return class_type;
		}
	}

	return Type::Unknown;
}

std::string BaseHandManifestation::type_to_str(BaseHandManifestation::Type type)
{
	switch(type) {
	case Type::Unknown:
		return "Unknown";

	case Type::GarageGasPumpHandle:
		return "GarageGasPumpHandle";

	case Type::Decal:
		return "Decal";

	case Type::Paint:
		return "Paint";

	case Type::AnchorRadar:
		return "AnchorRadar";

	case Type::BatteryCharger:
		return "BatteryCharger";

	case Type::Battery:
		return "Battery";

	case Type::BioLantern:
		return "BioLantern";

	case Type::Breaker:
		return "Breaker";

	case Type::Cutter:
		return "Cutter";

	case Type::Extractor:
		return "Extractor";

	case Type::FixBulb:
		return "FixBulb";

	case Type::FixElectronic:
		return "FixElectronic";

	case Type::FixMechanical:
		return "FixMechanical";

	case Type::FixSeal:
		return "FixSeal";

	case Type::ThrowFlare:
		return "ThrowFlare";

	case Type::FlareGun:
		return "FlareGun";

	case Type::BaseFlashLight:
		return "BaseFlashLight";

	case Type::CampingLight:
		return "CampingLight";

	case Type::Flashlight:
		return "Flashlight";

	case Type::HoldFood:
		return "HoldFood";

	case Type::FuelCan:
		return "FuelCan";

	case Type::HealerBasic:
		return "HealerBasic";

	case Type::HoldMedicine:
		return "HoldMedicine";

	case Type::HoldMedPack:
		return "HoldMedPack";

	case Type::PatchKit:
		return "PatchKit";

	case Type::Scrapper:
		return "Scrapper";

	case Type::Unlocker:
		return "Unlocker";

	case Type::Vacuum:
		return "Vacuum";

	case Type::ResourceGasPumpHandle:
		return "ResourceGasPumpHandle";

	default:
		break;
	}

	return "Parse error";
}
