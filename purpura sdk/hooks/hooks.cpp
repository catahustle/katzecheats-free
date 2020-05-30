#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include <intrin.h>

#pragma intrinsic(_ReturnAddress)

#include "hooks.h"
#include "minhook/minhook.h"

#include "../utilities/globals.h"
#include "../utilities/render_manager.h"
#include "../utilities/math.h"

#include "../menu/menu.h"
#include "../menu/settings.h"

#include "../features/visuals/visuals.h"
#include "../features/misc/misc.h"
#include "../features/engine_prediction/engine_prediction.h"
#include "../features/legit/legit_lag_comp.h"
#include "../features/legit/legit_aimbot.h"
#include "../features/legit/legit_antiaim.h"
#include "../features/legit/triggerbot.h"
#include "../features/skins/skinchanger.h"
#include "../features/misc/sfx/hitsounds.h"
#include "../features/rage/ragebot.h"

#include "../utilities/imgui/imgui.h"
#include "../utilities/imgui/imgui_impl_dx9.h"
#include "../utilities/imgui/imgui_impl_win32.h"

// i love macro so much
#define hook(base, index, detour, original) MH_CreateHook((LPVOID)(*(int**)base)[index], detour, &original)
#define def_orig(name) void* name = nullptr

def_orig(o_frame_stage_notify);
def_orig(o_create_move);
def_orig(o_viewmodel_fov);
def_orig(o_do_post_screen_effects);
def_orig(o_paint_traverse);
def_orig(o_lock_cursor);
def_orig(o_fire_event_client_side);
def_orig(o_draw_model_execute);
def_orig(o_sv_cheats);
def_orig(o_send_net_msg);

void hooks::init() {
	global_utils::console_log("initialising hooks...");

	MH_Initialize();
	hook(I::client, indexes::frame_stage_notify, frame_stage_notify_hook, o_frame_stage_notify);
	hook(I::client_mode, indexes::create_move, create_move_hook, o_create_move);
	hook(I::client_mode, indexes::viewmodel_fov, viewmodel_fov_hook, o_viewmodel_fov);
	hook(I::client_mode, indexes::do_post_screen_effects, do_post_screen_effects_hook, o_do_post_screen_effects);
	hook(I::panel, indexes::paint_traverse, paint_traverse_hook, o_paint_traverse);
	hook(I::surface, indexes::lock_cursor, lock_cursor_hook, o_lock_cursor);
	hook(I::gameeventmanager, indexes::fire_event_client_side, fire_event_client_side_hook, o_fire_event_client_side);
	hook(I::model_render, indexes::draw_model_execute, draw_model_execute_hook, o_draw_model_execute);
	hook(I::game_cvar->find_var("sv_cheats"), indexes::sv_cheats, sv_cheats_get_bool, o_sv_cheats);

	auto send_net_msg_addr = (LPVOID)global_utils::get_signature("engine.dll", "55 8B EC 83 EC 08 56 8B F1 8B 86 ? ? ? ? 85");
	MH_CreateHook(send_net_msg_addr, hooks::send_net_msg_hook, &o_send_net_msg);

	MH_EnableHook(MH_ALL_HOOKS);

	global_utils::console_log("hooks initialised, have fun!");

	present_addr = global_utils::get_signature("gameoverlayrenderer.dll", "68 ? ? ? ? 68 ? ? ? ? FF 76 44") + 1;
	reset_addr = global_utils::get_signature("gameoverlayrenderer.dll", "6A 01 68 ? ? ? ? 68 ? ? ? ? FF 76 40 E8 ? ? ? ? 83 C4 10 68 ? ? ? ? FF 76 44 E8 ? ? ? ? 83") + 3; // i made this without sigmaker because i forgot to install it
	original_present = **reinterpret_cast<decltype(&original_present)*>(present_addr);
	original_reset = **reinterpret_cast<decltype(&original_reset)*>(reset_addr);
	**reinterpret_cast<void***>(present_addr) = reinterpret_cast<void*>(&present_hook);
	**reinterpret_cast<void***>(reset_addr) = reinterpret_cast<void*>(&reset_hook);

	// i was too lazy to delete this
	interfaces::engine_client->unrestricted_client_cmd("clear");
	interfaces::engine_client->unrestricted_client_cmd("echo [katzecheats]: hi there");
}

void hooks::restore() {
	MH_DisableHook(MH_ALL_HOOKS);

	**reinterpret_cast<decltype(&original_present)*>(present_addr) = original_present;
	**reinterpret_cast<decltype(&original_reset)*>(reset_addr) = original_reset;

	Sleep(1000);

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if(m_OldWndProc) SetWindowLongA(m_hWindow, GWL_WNDPROC, m_OldWndProc);
	m_OldWndProc = 0;
}

extern vec3_t old_punch_angle;
vec3_t vangles;
bool __fastcall hooks::create_move_hook(i_client_mode* thisptr, void* edx, float sample_frametime, c_usercmd* cmd) {
	reinterpret_cast<create_move_t>(o_create_move)(thisptr, edx, sample_frametime, cmd);

	auto o_func = reinterpret_cast<create_move_t>(o_create_move)(thisptr, edx, sample_frametime, cmd);

	if (!interfaces::engine_client->in_game_connected() || !cmd || !cmd->command_number)
		return o_func;

	global_variables::global_cmd = cmd;
	global_variables::local_player = interfaces::client_ent_list->get_client_entity(interfaces::engine_client->get_local_player());

	if (!global_variables::local_player)
		return o_func;

	uintptr_t *frame_pointer;
	__asm mov frame_pointer, ebp;
	global_variables::b_send_packet = *(bool*)(*frame_pointer - 0x1C);

	if (c_config::misc::infinity_duck)
		cmd->buttons |= cmd_buttons::in_bullrush;

	if (c_config::rage::fakelag && c_config::rage::antiaims_enabled)
		G::b_send_packet = reinterpret_cast<i_net_channel_info*>(I::engine_client->get_net_channel_info())->chokedPackets > c_config::rage::fakelag;

	if (c_config::visuals::reveal_ranks && cmd->buttons & in_score)
		I::client->dispatch_user_msg(50);

	features::misc->on_create_move(global_variables::global_cmd);
	features::legit->autopistol();

	features::engine_pred->start_prediction(global_variables::local_player, global_variables::global_cmd);

	features::lag_comp->on_create_move();
	features::rage->on_create_move(); // ALARM PASTED FROM FATALITY AND LEAKED CSGO SOURCE CODE
	features::legit->on_create_move();
	features::triggerbot->on_create_move();
	
	global_variables::original_angles = cmd->viewangles;

	features::engine_pred->end_prediction(global_variables::local_player);

	features::rage->apply_aa();
	vangles = cmd->viewangles;
	features::legit_aa->on_create_move();

	global_math::normalize_angles(global_variables::global_cmd->viewangles);
	global_math::correct_movement(cmd, global_variables::original_angles);

	cmd->buttons &= ~(in_moveleft | in_moveright | in_forward | in_back);

	*(bool*)(*frame_pointer - 0x1C) = global_variables::b_send_packet;

	old_punch_angle = global_variables::local_player->aim_punch_angle() * 2.f;

	if (G::b_send_packet) // hmmmmmmmmmmmmmmm
		c_config::visuals::fake = cmd->viewangles;
	else
		c_config::visuals::real = cmd->viewangles;

	if (I::input_system->is_button_down(button_code_t::KEY_LSHIFT) && G::local_player->velocity().Length2D() > 5.0f && c_config::misc::slowwalk) // ok
	{
		cmd->buttons &= ~in_walk;
		cmd->sidemove *= c_config::misc::slowwalk_speed / 100.0f;
		cmd->forwardmove *= c_config::misc::slowwalk_speed / 100.0f;
		cmd->upmove *= c_config::misc::slowwalk_speed / 100.0f;
	}

	return false;
}

float __stdcall hooks::viewmodel_fov_hook() {
	if (c_config::visuals::viewmodel_fov == 0)
		return 68.f;

	return c_config::visuals::viewmodel_fov;
}

void __fastcall hooks::paint_traverse_hook(PVOID p_panel, int edx, unsigned int vgui_panel, bool force_repaint, bool allow_force) {
	static unsigned int panel_zoom_id = 0;

	if (!panel_zoom_id)
		if (!strcmp("HudZoom", I::panel->get_name(vgui_panel)))
			panel_zoom_id = vgui_panel;

	if (panel_zoom_id == vgui_panel && c_config::visuals::remove_scope)
		return;

	reinterpret_cast<paint_traverse_t>(o_paint_traverse)(p_panel, vgui_panel, force_repaint, allow_force);
}

void __fastcall hooks::lock_cursor_hook(i_surface* thisptr, void* edx) {
	if (!menu.menu_opened)
		return reinterpret_cast<lock_cursor_t>(o_lock_cursor)(thisptr, edx);

	interfaces::surface->unlock_cursor();
}

HRESULT __stdcall hooks::present_hook(IDirect3DDevice9* pDevice, const RECT* src, const RECT* dest, HWND wnd_override, const RGNDATA* dirty_region)
{
	static bool bWasInitialized = false;
	if (!bWasInitialized)
	{
		D3DDEVICE_CREATION_PARAMETERS params;
		pDevice->GetCreationParameters(&params);
		m_hWindow = params.hFocusWindow;
		m_OldWndProc = SetWindowLongA(m_hWindow, GWL_WNDPROC, (LONG)wndproc_hook);

		ImGui::CreateContext();
		ImGui_ImplWin32_Init(m_hWindow);
		ImGui_ImplDX9_Init(pDevice);
		menu.setup_styles();

		render_manager::init();

		bWasInitialized = true;
	}

	IDirect3DVertexDeclaration9* vertDec;
	IDirect3DVertexShader9* vertShader;
	DWORD dwOld_D3DRS_COLORWRITEENABLE;

	pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &dwOld_D3DRS_COLORWRITEENABLE);
	pDevice->GetVertexDeclaration(&vertDec);
	pDevice->GetVertexShader(&vertShader);
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
	pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);
		
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	render_manager::draw_list = ImGui::GetBackgroundDrawList();

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

	ImGui::PushFont(render_manager::fonts::esp_font);

	features::vis->render();

	ImGui::PopFont();

	int w, h;
	I::engine_client->get_screen_size(w, h);
	auto local = I::client_ent_list->get_client_entity(I::engine_client->get_local_player());
	if (local && c_config::visuals::remove_scope)
	{
		if (local->is_alive() && local->scoped())
		{
			auto weapon = local->active_weapon();
			if (weapon)
			{
				auto data = weapon->get_cs_weapon_data();
				if (data)
				{
					if (data->iWeaponType == cs_weapon_type::WEAPONTYPE_SNIPER_RIFLE)
					{
						render_manager::draw_list->AddLine(ImVec2(w / 2, 0), ImVec2(w / 2, h), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)));
						render_manager::draw_list->AddLine(ImVec2(0, h / 2), ImVec2(w, h / 2), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)));
					}
				}
			}
		}
	}

	{
		static bool last_state = menu.menu_opened;
		static int alpha = 0;
		if (last_state != menu.menu_opened)
		{
			if (!last_state)
				alpha += 1;
			else
				alpha -= 1;

			last_state = menu.menu_opened;
		}

		if (alpha != 0 && alpha != 500)
		{
			if (menu.menu_opened) alpha += 10;
			else alpha -= 10;
		}

		if (alpha > 500) alpha = 500;
		if (alpha < 0) alpha = 0;

		render_manager::draw_list->AddRectFilled(ImVec2(0, 0), ImVec2(w, h), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, (float)alpha / 1000.0f)));
	}

	menu.draw();

	ImGui::EndFrame();

	ImGui::Render();

	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, dwOld_D3DRS_COLORWRITEENABLE);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
	pDevice->SetVertexDeclaration(vertDec);
	pDevice->SetVertexShader(vertShader);
			
	return original_present(pDevice, src, dest, wnd_override, dirty_region);
}

HRESULT __stdcall hooks::reset_hook(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	auto hr = original_reset(device, pPresentationParameters);

	if (hr >= 0)
		ImGui_ImplDX9_CreateDeviceObjects();

	return hr;
}

bool process_bind(c_config::bind& key)
{
	bool old = key.state;
	if (key.mode)
		key.state = !key.state;
	return old != key.state;
}

bool process_kboard_msg(const WPARAM& wParam, c_config::bind& key)
{
	if (wParam == key.key)
		return process_bind(key);
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall hooks::wndproc_hook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_KEYDOWN) // shit, this is the best keybind system
	{
		switch (wParam)
		{
		case VK_DELETE: g_unload = true; break;
		case VK_INSERT:
			menu.menu_opened = !menu.menu_opened;
			if (menu.menu_opened && c_config::legit::auto_pick && interfaces::engine_client->in_game_connected())
			{
				c_base_entity* local = interfaces::client_ent_list->get_client_entity(interfaces::engine_client->get_local_player());
				if (local)
				{
					if (local->is_alive())
					{
						c_base_combat_weapon* weapon = local->active_weapon();
						if (weapon)
						{
							c_config::legit::CWeapon = weapon->get_config_id();
							if (c_config::legit::CWeapon > 34) c_config::legit::CWeapon = 0;
						}
					}
				}
			}
			break;
		default:
			if (wParam)
			{
				process_kboard_msg(wParam, c_config::legit::autofire);
				process_kboard_msg(wParam, c_config::misc::legit_aa_inverter);
				process_kboard_msg(wParam, c_config::triggerbot::key);
				process_kboard_msg(wParam, c_config::visuals::thirdperson_key);

				c_config::rage::manual_back.mode = 1;
				c_config::rage::manual_left.mode = 1;
				c_config::rage::manual_right.mode = 1;

				if (process_kboard_msg(wParam, c_config::rage::manual_back) && c_config::rage::manual_back.state)
					c_config::rage::manual_left.state = c_config::rage::manual_right.state = 0;

				if (process_kboard_msg(wParam, c_config::rage::manual_left) && c_config::rage::manual_left.state)
					c_config::rage::manual_back.state = c_config::rage::manual_right.state = 0;

				if (process_kboard_msg(wParam, c_config::rage::manual_right) && c_config::rage::manual_right.state)
					c_config::rage::manual_left.state = c_config::rage::manual_back.state = 0;
			}
		}
	}

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam) && menu.menu_opened)
		return true;

	if (menu.menu_opened) return true;

	return CallWindowProcA((WNDPROC)m_OldWndProc, hwnd, msg, wParam, lParam);
}

bool __fastcall hooks::fire_event_client_side_hook(void* thisptr, void* edx, i_game_event* event)
{
	if (event)
	{
		if (!strcmp(event->get_name(), "player_death"))
		{
			if (I::engine_client->get_player_for_user_id(event->get_int("attacker")) == I::engine_client->get_local_player())
			{
				auto local = I::client_ent_list->get_client_entity(I::engine_client->get_local_player());
				if (!local) return reinterpret_cast<fire_event_client_side_t>(o_fire_event_client_side)(thisptr, edx, event);
				auto weapon = local->active_weapon();
				if (!weapon) return reinterpret_cast<fire_event_client_side_t>(o_fire_event_client_side)(thisptr, edx, event);
				if(c_config::legit::weapons[weapon->get_config_id()].kill_delay) features::legit->stop_time = I::global_vars->curtime + ((float)(c_config::legit::weapons[weapon->get_config_id()].kill_delay) / 1000.0f);
			}
		}
		else if (!strcmp(event->get_name(), "player_hurt"))
		{
			if (I::engine_client->get_player_for_user_id(event->get_int("attacker")) == I::engine_client->get_local_player())
			{
				c_base_entity* ent = I::client_ent_list->get_client_entity(I::engine_client->get_player_for_user_id(event->get_int("userid")));
				if (ent)
					if (!ent->dormant() && ent->get_client_class()->class_id == ClassId_CCSPlayer)
						G::ShotsHit[ent->ent_index()]++; // HAHHAHH i dont know what was wrong with me
			}
		}
	}

	return reinterpret_cast<fire_event_client_side_t>(o_fire_event_client_side)(thisptr, edx, event);
}

void __fastcall hooks::draw_model_execute_hook(void* thisptr, int edx, i_mat_render_context* ctx, const draw_model_state_t& state, const model_render_info_t& info, matrix3x4_t* matrix)
{
	if((info.flags & 0x40000000) != 0)
		return reinterpret_cast<draw_model_execute_t>(o_draw_model_execute)(thisptr, edx, ctx, state, info, matrix);

	if (features::vis->render_chams(reinterpret_cast<draw_model_execute_t>(o_draw_model_execute), edx, ctx, state, info, matrix))
		reinterpret_cast<draw_model_execute_t>(o_draw_model_execute)(thisptr, edx, ctx, state, info, matrix);

	interfaces::model_render->override_material(nullptr);
}

void proc_bind(c_config::bind& key)
{
	bool down = I::input_system->is_button_down(I::input_system->vkey_to_button_code(key.key));
	if (!key.mode)
		key.state = down;
}

void __fastcall hooks::frame_stage_notify_hook(void* thisptr, int edx, client_frame_stage_t stage)
{
	if (stage == FRAME_START)
	{
		proc_bind(c_config::legit::autofire);
		proc_bind(c_config::triggerbot::key);
		proc_bind(c_config::visuals::thirdperson_key);
		proc_bind(c_config::misc::legit_aa_inverter);		
	}

	if (stage == FRAME_RENDER_START)
	{
		if (c_config::misc::legit_aa || c_config::rage::antiaims && c_config::rage::desync)
		{
			auto local = I::client_ent_list->get_client_entity(I::engine_client->get_local_player());
			if (local)
			{
				auto animstate = local->get_anim_state();
				if (animstate)
					animstate->m_flGoalFeetYaw = c_config::visuals::real.y; // *visible confusion*
			}
		}

		features::vis->thirdperson();
		features::legit_aa->resolve(); // legit aa resolver with bruteforce, ok
	}

	if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		features::skins->apply();
	}

	reinterpret_cast<frame_stage_notify_t>(o_frame_stage_notify)(thisptr, edx, stage);
}

int __fastcall hooks::do_post_screen_effects_hook(void* thisptr, int a2, int a3)
{
	if (I::engine_client->in_game_connected())
	{
		features::vis->modify_smoke();
		features::vis->nightmode();
	}

	return reinterpret_cast<do_post_screen_effects_t>(o_do_post_screen_effects)(thisptr, a2, a3);
}

void __cdecl hooks::sequence_hook(const c_recv_proxy_data* const_proxy_data, void* ent, void* output)
{
	static auto o_fn = sequence_hk->get_original_function();

	const auto proxy_data = const_cast<c_recv_proxy_data*>(const_proxy_data);
	const auto view_model = (c_base_view_model*)ent;
	features::skins->fix_sequence(proxy_data, view_model);

	o_fn(proxy_data, ent, output);
}

bool __fastcall hooks::sv_cheats_get_bool(void* convar, void* edx)
{
	static auto dwCAM_Think = global_utils::get_signature("client_panorama.dll", "85 C0 75 30 38 86");

	if (reinterpret_cast<DWORD>(_ReturnAddress()) == dwCAM_Think)
		return true;

	return reinterpret_cast<bool(__thiscall*)(void*)>(o_sv_cheats)(convar);
}

bool __fastcall hooks::send_net_msg_hook(void* thisptr, int edx, i_net_message& msg, bool force_reliable, bool voice)
{
	if (msg.get_type() == 14 && c_config::misc::svpure_bypass)
		return false;

	if (msg.get_group() == 9)
		voice = true;

	return reinterpret_cast<send_net_msg_t>(o_send_net_msg)(thisptr, edx, msg, force_reliable, voice);
}