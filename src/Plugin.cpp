#include <codecvt>
#include <locale>
#include <memory>
#include <mutex>
#include <sstream>

#include <Windows.h>

// only really necessary if you want to render to the screen
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"

#include "rendering/d3d11.hpp"
#include "rendering/d3d12.hpp"

#include "uevr/Plugin.hpp"

#include "pdsdk/MainCharacter.hpp"
#include "pdsdk/MainCharacterManager.hpp"
#include "pdsdk/PlayerCarIntro.hpp"
#include "pdsdk/PlayerCarNew.hpp"

#include "Math.hpp"

using namespace uevr;

class PacificDrivePlugin : public uevr::Plugin
{
  public:
	PacificDrivePlugin() = default;

	void on_initialize() override
	{
		ImGui::CreateContext();
	}

	void on_present() override
	{
		std::scoped_lock _{m_imgui_mutex};

		if(!m_initialized) {
			if(!initialize_imgui()) {
				API::get()->log_info("Failed to initialize imgui");
				return;
			} else {
				API::get()->log_info("Initialized imgui");
			}
		}

		const auto renderer_data = API::get()->param()->renderer;

		if(!API::get()->param()->vr->is_hmd_active()) {
			if(!m_was_rendering_desktop) {
				m_was_rendering_desktop = true;
				on_device_reset();
				return;
			}

			m_was_rendering_desktop = true;

			if(renderer_data->renderer_type == UEVR_RENDERER_D3D11) {
				ImGui_ImplDX11_NewFrame();
				g_d3d11.render_imgui();
			} else if(renderer_data->renderer_type == UEVR_RENDERER_D3D12) {
				auto command_queue = (ID3D12CommandQueue *)renderer_data->command_queue;

				if(command_queue == nullptr) {
					return;
				}

				ImGui_ImplDX12_NewFrame();
				g_d3d12.render_imgui();
			}
		}
	}

	void on_device_reset() override
	{
		std::scoped_lock _{m_imgui_mutex};

		const auto renderer_data = API::get()->param()->renderer;

		if(renderer_data->renderer_type == UEVR_RENDERER_D3D11) {
			ImGui_ImplDX11_Shutdown();
			g_d3d11 = {};
		}

		if(renderer_data->renderer_type == UEVR_RENDERER_D3D12) {
			g_d3d12.reset();
			ImGui_ImplDX12_Shutdown();
			g_d3d12 = {};
		}

		m_initialized = false;
	}

	void on_post_render_vr_framework_dx11(ID3D11DeviceContext *context, ID3D11Texture2D *texture,
										  ID3D11RenderTargetView *rtv) override
	{
		const auto vr_active = API::get()->param()->vr->is_hmd_active();

		if(!m_initialized || !vr_active) {
			return;
		}

		if(m_was_rendering_desktop) {
			m_was_rendering_desktop = false;
			on_device_reset();
			return;
		}

		std::scoped_lock _{m_imgui_mutex};

		ImGui_ImplDX11_NewFrame();
		g_d3d11.render_imgui_vr(context, rtv);
	}

	void on_post_render_vr_framework_dx12(ID3D12GraphicsCommandList *command_list,
										  ID3D12Resource *rt,
										  D3D12_CPU_DESCRIPTOR_HANDLE *rtv) override
	{
		const auto vr_active = API::get()->param()->vr->is_hmd_active();

		if(!m_initialized || !vr_active) {
			return;
		}

		if(m_was_rendering_desktop) {
			m_was_rendering_desktop = false;
			on_device_reset();
			return;
		}

		std::scoped_lock _{m_imgui_mutex};

		ImGui_ImplDX12_NewFrame();
		g_d3d12.render_imgui_vr(command_list, rtv);
	}

	bool on_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override
	{
		ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);

		return !ImGui::GetIO().WantCaptureMouse && !ImGui::GetIO().WantCaptureKeyboard;
	}

	void on_pre_engine_tick(API::UGameEngine *engine, float delta) override
	{
		plugin_on_pre_engine_tick(engine, delta);

		if(m_initialized) {
			std::scoped_lock _{m_imgui_mutex};

			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			internal_frame();

			ImGui::EndFrame();
			ImGui::Render();
		}
	}

  private:
	enum CarEnterExit
	{
		Enter,
		Exit
	};

	enum AimMethod : int32_t
	{
		GAME,
		HEAD,
		RIGHT_CONTROLLER,
		LEFT_CONTROLLER,
		TWO_HANDED_RIGHT,
		TWO_HANDED_LEFT,
	};

	enum AimHand
	{
		LEFT = 0,
		RIGHT
	};

	bool initialize_imgui()
	{
		if(m_initialized) {
			return true;
		}

		std::scoped_lock _{m_imgui_mutex};

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		static const auto imgui_ini =
			API::get()->get_persistent_dir(L"pacific_drive_plugin.ini").string();
		ImGui::GetIO().IniFilename = imgui_ini.c_str();

		const auto renderer_data = API::get()->param()->renderer;

		DXGI_SWAP_CHAIN_DESC swap_desc{};
		auto swapchain = (IDXGISwapChain *)renderer_data->swapchain;
		swapchain->GetDesc(&swap_desc);

		m_wnd = swap_desc.OutputWindow;

		if(!ImGui_ImplWin32_Init(m_wnd)) {
			return false;
		}

		if(renderer_data->renderer_type == UEVR_RENDERER_D3D11) {
			if(!g_d3d11.initialize()) {
				return false;
			}
		} else if(renderer_data->renderer_type == UEVR_RENDERER_D3D12) {
			if(!g_d3d12.initialize()) {
				return false;
			}
		}

		m_initialized = true;
		return true;
	}

	void internal_frame()
	{

		const auto vr = API::get()->param()->vr;

		if(ImGui::Begin("Pacific Drive Plugin")) {
			ImGui::Text("is_aim_allowed(): %u", vr->is_aim_allowed());
			ImGui::Text("get_aim_method(): %u", vr->get_aim_method());
			ImGui::Text("m_player_in_car: %u", m_player_in_car);
			ImGui::Text("m_force_player_in_car: %u", m_force_player_in_car);
			ImGui::Text("Angle X:%.2f Y:%.2f Z:%.2f", m_euler.x, m_euler.y, m_euler.z);
			ImGui::Text("Delta X:%.2f Y:%.2f Z:%.2f", m_euler_delta.x, m_euler_delta.y,
						m_euler_delta.z);
			ImGui::Text("active item: %ls", m_active_item_name.c_str());
		}
	}

	template <PlayerCarConcept T>
	void plugin_car_autostart_on_car_enter_exit(PlayerCarInterface<T> *const player_car,
												CarEnterExit state)
	{
		if(state == CarEnterExit::Enter) {
			player_car->toggle_ignition_state(PlayerCar::IgnitionState::On);
			player_car->toggle_gearbox_state(PlayerCar::GearBoxState::Drive);
		} else {
			player_car->toggle_ignition_state(PlayerCar::IgnitionState::Off);
			player_car->toggle_gearbox_state(PlayerCar::GearBoxState::Park);
		}
	}

	template <PlayerCarConcept T>
	void plugin_car_controller_aim_on_car_enter_exit(PlayerCarInterface<T> *const player_car,
													 CarEnterExit state)
	{
		if(state == CarEnterExit::Enter) {
		} else {
			// prevent on foot camera from getting misaligned
			player_car->reset_camera();
		}
	}

	template <PlayerCarConcept T>
	void process_car_enter_exit(MainCharacter *const main_character,
								PlayerCarInterface<T> *const player_car, float delta)
	{
		bool current_player_in_car = main_character->is_player_in_car() || m_force_player_in_car;

		if(current_player_in_car != m_player_in_car) {
			if(current_player_in_car) {
				API::get()->log_info("CarEnterExit::Enter");

				// player entered car event
				plugin_car_controller_aim_on_car_enter_exit(player_car, CarEnterExit::Enter);
				plugin_car_autostart_on_car_enter_exit(player_car, CarEnterExit::Enter);
			} else {
				API::get()->log_info("CarEnterExit::Exit");

				// player exited car event
				plugin_car_controller_aim_on_car_enter_exit(player_car, CarEnterExit::Exit);
				plugin_car_autostart_on_car_enter_exit(player_car, CarEnterExit::Exit);
			}
		}

		m_player_in_car = current_player_in_car;
	}

	template <PlayerCarConcept T>
	void process_controller_aim(MainCharacter *const main_character,
								PlayerCarInterface<T> *const player_car, float delta)
	{
		glm::vec3 og_controller_pos{};
		glm::quat og_controller_rot{};
		glm::quat rotation_offset{};
		glm::vec3 right_controller_forward{};
		glm::vec3 standing_origin{};

		bool player_in_car = main_character->is_player_in_car() || m_force_player_in_car;

		const auto vr = API::get()->param()->vr;

		const auto aim_method = (AimMethod)vr->get_aim_method();
		bool is_using_controller =
			(aim_method == AimMethod::LEFT_CONTROLLER || aim_method == AimMethod::RIGHT_CONTROLLER);

		const auto controller_index = (aim_method == AimMethod::RIGHT_CONTROLLER)
										  ? vr->get_right_controller_index()
										  : vr->get_left_controller_index();

		vr->get_aim_pose(controller_index, (UEVR_Vector3f *)&og_controller_pos,
						 (UEVR_Quaternionf *)&og_controller_rot);
		vr->get_rotation_offset((UEVR_Quaternionf *)&rotation_offset);

		right_controller_forward = og_controller_rot * glm::vec3{0.0f, 0.0f, 1.0f};

		// We need to construct a sightline from the standing origin to the direction the controller
		// is facing This is so the camera will be facing a more correct direction rather than the
		// raw controller rotation
		const auto right_controller_end = og_controller_pos + (right_controller_forward * 1000.0f);

		vr->get_standing_origin((UEVR_Vector3f *)&standing_origin);
		const auto adjusted_forward = glm::normalize(right_controller_end - standing_origin);
		const auto target_forward = utility::math::to_quat(adjusted_forward);

		const auto controller_rot = glm::normalize(rotation_offset * target_forward);

		auto spherical_distance = glm::dot(controller_rot, m_last_aim_rot);
		if(spherical_distance < 0.0f) {
			// we do this because we want to rotate the shortest distance
			spherical_distance = -spherical_distance;
		}
		const auto lerped_rotation = glm::slerp(m_last_aim_rot, controller_rot,
												std::min(delta, 0.1f) * 15.0f * spherical_distance);
		m_last_aim_rot = controller_rot;

		auto m_euler_new = glm::degrees(utility::math::euler_angles_from_steamvr(lerped_rotation));

		if(m_last_aim_method == aim_method) {
			m_euler_delta.x = m_euler_new.x - m_euler.x;
			m_euler_delta.y = m_euler_new.y - m_euler.y;
			m_euler_delta.z = m_euler_new.z - m_euler.z;
		} else {
			m_euler_delta = {};
		}

		if(player_in_car) {
			vr->set_aim_allowed(false);

			if(is_using_controller) {
				player_car->look_left_right(m_euler_delta.y);
				player_car->look_up_down(-m_euler_delta.x);
			}
		}

		m_euler = m_euler_new;
		m_last_aim_method = aim_method;
	}

	template <PlayerCarConcept T>
	void process_hand_items(MainCharacter *const main_character,
							PlayerCarInterface<T> *const player_car, float delta)
	{
		const auto item = main_character->get_active_hand_item();

		if(!item) {
			m_active_item_name = {};
			return;
		}

		const auto item_name = item->get_full_name();

		if(m_active_item_name != item_name) {
			API::get()->log_info("Item equipped: %ls", item_name.c_str());

			const auto new_scale = item->get_scale();
			API::get()->log_info("Type: %s scale X:%f, y:%f, z:%f",
								 BaseHandManifestation::type_to_str(item->get_type()).c_str(),
								 new_scale.x, new_scale.y, new_scale.z);

			item->set_actor_scale3d(&new_scale);

			const auto aim_method = API::get()->param()->vr->get_aim_method();

			API::UObject *attach_component = nullptr;

			const auto static_mesh_component_class =
				API::get()->find_uobject<API::UClass>(L"Class /Script/Engine.StaticMeshComponent");
			if(static_mesh_component_class) {
				const auto components = item->get_components_by_class(static_mesh_component_class);

				if(components.size()) {
					attach_component = components[0];
				}
			} else {
				API::get()->log_error("process_hand_items: static_mesh_component_class NULL");
			}

			if(attach_component) {
				API::get()->log_info("attach_component: %ls",
									 attach_component->get_full_name().c_str());

				auto mc_state =
					API::UObjectHook::get_or_add_motion_controller_state(attach_component);
				if(mc_state) {
					mc_state->set_hand((aim_method == AimMethod::RIGHT_CONTROLLER) ? AimHand::RIGHT
																				   : AimHand::LEFT);

					const auto rot = item->get_rotation_offset();
					API::get()->log_info("Rotation quat w:%f, x:%f, y:%f, z:%f", rot.w, rot.x,
										 rot.y, rot.z);
					mc_state->set_rotation_offset(&rot);

					const auto offset = item->get_position_offset();
					mc_state->set_location_offset(&offset);

					mc_state->set_permanent(true);
				} else {
					API::get()->log_error("process_hand_items: mc_state NULL");
				}
			} else {
				API::get()->log_error("process_hand_items: attach_component NULL");
			}
		}

		m_active_item_name = item_name;
	}

	template <PlayerCarConcept T>
	void process_on_level_load(MainCharacter *const main_character,
							   PlayerCarInterface<T> *const player_car, float delta)
	{
		const auto main_character_manager = MainCharacterManager::get_instance();
		if(!main_character_manager) {
			return;
		}

		const auto main_character_instance = main_character->get_full_name();
		const auto player_car_instance = player_car->get_full_name();

		if((main_character_instance != m_last_main_character_instance) ||
		   (player_car_instance != m_last_player_car_instance)) {
			// new level or save game has been loaded

			API::get()->log_info("process_on_level_load");

			if(main_character_manager->get_main_character_movement_mode() ==
			   MainCharacterManager::ALS_MovementMode::Car) {
				// hack around get_player_in_car() not working on zone load
				API::get()->log_info("load_in_car_hack: activated");

				m_force_player_in_car = true;
			} else {
				API::get()->log_info("load_in_car_hack: not needed");

				m_force_player_in_car = false;
			}

			if(m_force_player_in_car && player_car_instance.contains(L"BP_PlayerCar_Intro")) {
				// don't trigger on car enter event in intro
				m_player_in_car = true;
			} else {
				m_player_in_car = false;
			}

			m_active_item_name = {};
			m_last_aim_method = AimMethod::GAME;
			m_euler_delta = {};
		}

		if(m_force_player_in_car && (main_character_manager->get_main_character_movement_mode() !=
									 MainCharacterManager::ALS_MovementMode::Car)) {
			// player got out of the car for the first time, disable the hack
			API::get()->log_info("load_in_car_hack: deactivated");

			m_force_player_in_car = false;
		}

		m_last_main_character_instance = main_character_instance;
		m_last_player_car_instance = player_car_instance;
	}

	template <PlayerCarConcept T>
	void plugin_on_pre_engine_tick_inner(MainCharacter *const main_character,
										 PlayerCarInterface<T> *const player_car, float delta)
	{
		process_on_level_load(main_character, player_car, delta);
		process_car_enter_exit(main_character, player_car, delta);
		process_controller_aim(main_character, player_car, delta);
		process_hand_items(main_character, player_car, delta);
	}

	void plugin_on_pre_engine_tick(API::UGameEngine *engine, float delta)
	{
		const auto main_character = MainCharacter::get_instance();
		if(!main_character) {
			API::get()->log_error("MainCharacter::get_instance() returned NULL");
			return;
		}

		const auto player_car_new = PlayerCarNew::get_instance();
		const auto player_car_intro = PlayerCarIntro::get_instance();

		if(!player_car_new && !player_car_intro) {
			API::get()->log_error("PlayerCarNew::get_instance() and PlayerCarIntro::get_instance() "
								  "both returned NULL");

			// force to look for PlayerCarIntro class so we can find instance on
			// normal gameplay -> menu -> intro transition
			PlayerCarIntro::static_class(true);

			return;
		}

		if(player_car_new) {
			plugin_on_pre_engine_tick_inner(main_character, player_car_new, delta);
		} else {
			plugin_on_pre_engine_tick_inner(main_character, player_car_intro, delta);
		}
	}

  private:
	HWND m_wnd{};
	bool m_initialized{false};
	bool m_was_rendering_desktop{false};

	std::recursive_mutex m_imgui_mutex{};

	glm::vec3 m_euler{};
	glm::vec3 m_euler_delta{};
	glm::quat m_last_aim_rot{};

	bool m_player_in_car = false;
	bool m_force_player_in_car = false;

	AimMethod m_last_aim_method = AimMethod::GAME;

	std::wstring m_active_item_name{};

	std::wstring m_last_main_character_instance{};
	std::wstring m_last_player_car_instance{};
};

// Actually creates the plugin. Very important that this global is created.
// The fact that it's using std::unique_ptr is not important, as long as the constructor is called
// in some way.
std::unique_ptr<PacificDrivePlugin> g_plugin{new PacificDrivePlugin()};
