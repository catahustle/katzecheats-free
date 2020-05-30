#pragma once

#include "../interfaces/interfaces.h"

#include "../sdk/c_input.h"
#include "../utilities/recv_proxy_hook.h"

#include <d3d9.h>
#include <d3dx9.h>

namespace hooks {
	enum indexes {
		scene_end = 9,
		fire_event_client_side = 9,
		sv_cheats = 13,
		draw_model_execute = 21,
		create_move = 24,
		viewmodel_fov = 35,
		frame_stage_notify = 37,
		paint_traverse = 41,
		do_post_screen_effects = 44,
		lock_cursor = 67
	};

	void init();
	void restore();

	inline HWND m_hWindow = 0;
	inline LONG m_OldWndProc = 0;
	inline bool g_unload = 0;
	inline DWORD present_addr = 0;
	inline DWORD reset_addr = 0;
	inline recv_prop_hook* sequence_hk = nullptr;

	bool    __fastcall  create_move_hook(i_client_mode*, void*, float, c_usercmd*);
	void	__fastcall  paint_traverse_hook(PVOID, int, unsigned int, bool, bool);
	void    __fastcall  lock_cursor_hook(i_surface *, void *);
	float   __stdcall   viewmodel_fov_hook();
	HRESULT	__stdcall	present_hook(IDirect3DDevice9* pDevice, const RECT* src, const RECT* dest, HWND wnd_override, const RGNDATA* dirty_region);
	HRESULT	__stdcall	reset_hook(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters);
	LRESULT __stdcall	wndproc_hook(HWND, UINT, WPARAM, LPARAM);
	bool	__fastcall	fire_event_client_side_hook(void*, void*, i_game_event*);
	void	__fastcall	draw_model_execute_hook(void*, int, i_mat_render_context*, const draw_model_state_t&, const model_render_info_t&, matrix3x4_t*);
	void	__fastcall	frame_stage_notify_hook(void*, int, client_frame_stage_t);
	int		__fastcall	do_post_screen_effects_hook(void*, int, int);
	void	__cdecl		sequence_hook(const c_recv_proxy_data*, void*, void*);
	bool	__fastcall	sv_cheats_get_bool(void*, void*);
	bool	__fastcall	send_net_msg_hook(void*, int, i_net_message&, bool, bool);

	typedef bool(__fastcall* create_move_t) (i_client_mode*, void*, float, c_usercmd*);
	typedef void(__thiscall* paint_traverse_t) (PVOID, unsigned int, bool, bool);
	typedef void(__fastcall* lock_cursor_t) (i_surface*, void*);
	typedef HRESULT(__stdcall* present_t)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
	typedef HRESULT(__stdcall* reset_t)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
	typedef bool(__fastcall* fire_event_client_side_t)(void*, void*, i_game_event*);
	typedef void(__fastcall* draw_model_execute_t)(void*, int, i_mat_render_context*, const draw_model_state_t&, const model_render_info_t&, matrix3x4_t*);
	typedef void(__fastcall* frame_stage_notify_t)(void*, int, client_frame_stage_t);
	typedef int(__fastcall* do_post_screen_effects_t)(void*, int, int);
	typedef bool(__fastcall* sv_cheats_get_bool_t)(void*, void*);
	typedef bool(__fastcall* send_net_msg_t)(void*, int, i_net_message&, bool, bool);

	inline present_t original_present = nullptr;
	inline reset_t original_reset = nullptr;
}