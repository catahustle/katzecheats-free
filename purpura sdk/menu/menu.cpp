#include <vector>
#include <locale>

#include "menu.h"
#include "settings.h"

#include "../utilities/imgui/imgui.h"
#include "../utilities/Vinnitsya.h"

#include "configs/configs.h"

#include "../utilities/imgui/imgui.h"
#include "../utilities/imgui/imgui_internal.h"
#include "../utilities/imgui/ComboFilter.h"

#include "../features/skins/skinchanger.h"

c_menu menu;

#define cw c_config::legit::CWeapon

void c_menu::setup_styles()
{
	ImGuiIO& io = ImGui::GetIO();
	Font = io.Fonts->AddFontFromMemoryTTF((void*)vinnitsya_data, vinnitsya_size, 18, 0, io.Fonts->GetGlyphRangesCyrillic());
	Font25 = io.Fonts->AddFontFromMemoryTTF((void*)vinnitsya_data, vinnitsya_size, 25, 0, io.Fonts->GetGlyphRangesCyrillic());
	Font27 = io.Fonts->AddFontFromMemoryTTF((void*)vinnitsya_data, vinnitsya_size, 27, 0, io.Fonts->GetGlyphRangesCyrillic());

	ImGuiStyle& style = ImGui::GetStyle();
	style.FrameRounding = 6;
	style.WindowBorderSize = 0;
	style.WindowRounding = 7;
	style.PopupRounding = 7;
	style.GrabRounding = 7;
	style.WindowTitleAlign.x = 0.5f;
	
	ImVec4* colors = style.Colors;
	colors[ImGuiCol_Text] = { 1, 1, 1, 1 };
	colors[ImGuiCol_WindowBg] = { 24.0f / 255.0f, 35.0f / 255.0f, 75.0f / 255.0f, 1.0f };
	colors[ImGuiCol_PopupBg] = ImVec4(0.29f, 0.11f, 0.32f, 0.94f);
	colors[ImGuiCol_Button] = ImVec4(0.19f, 0.18f, 0.73f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.24f, 0.90f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.27f, 0.26f, 1.00f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.49f, 0.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.49f, 0.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.49f, 0.00f, 1.00f, 1.00f);
}

void c_menu::active_tab()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Button] = ImVec4(0.69f, 0.18f, 0.73f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.88f, 0.21f, 0.92f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.95f, 0.26f, 1.00f, 1.00f);
}

void c_menu::inactive_tab()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;
	colors[ImGuiCol_Button] = { 24.0f / 255.0f, 35.0f / 255.0f, 75.0f / 255.0f, 1.0f };
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.88f, 0.21f, 0.92f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.95f, 0.26f, 1.00f, 1.00f);
}

void c_menu::reset_buttons()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;
	colors[ImGuiCol_Button] = ImVec4(0.19f, 0.18f, 0.73f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.24f, 0.90f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.27f, 0.26f, 1.00f, 1.00f);
}

namespace ImGui
{
	bool ColorPicker(const char* label, color* c)
	{
		float Color[] = { c->r / 255.0f, c->g / 255.0f, c->b / 255.0f, c->a / 255.0f };
		bool ret = ImGui::ColorEdit4(label, Color, ImGuiColorEditFlags_NoInputs);
		*c = color(Color[0] * 255.0f, Color[1] * 255.0f, Color[2] * 255.0f, Color[3] * 255.0f);
		return ret;
	}	
}

const char* KeyNames[] = {
	"Unknown",
	"Not supported",
	"Not supported",
	"Not supported",
	"Not supported",
	"Not supported",
	"Not supported",
	"Unknown",
	"VK_BACK",
	"VK_TAB",
	"Unknown",
	"Unknown",
	"VK_CLEAR",
	"VK_RETURN",
	"Unknown",
	"Unknown",
	"VK_SHIFT",
	"VK_CONTROL",
	"Not supported",
	"VK_PAUSE",
	"VK_CAPITAL",
	"VK_KANA",
	"Unknown",
	"VK_JUNJA",
	"VK_FINAL",
	"VK_KANJI",
	"Unknown",
	"VK_ESCAPE",
	"VK_CONVERT",
	"VK_NONCONVERT",
	"VK_ACCEPT",
	"VK_MODECHANGE",
	"VK_SPACE",
	"VK_PRIOR",
	"VK_NEXT",
	"VK_END",
	"VK_HOME",
	"VK_LEFT",
	"VK_UP",
	"VK_RIGHT",
	"VK_DOWN",
	"VK_SELECT",
	"VK_PRINT",
	"VK_EXECUTE",
	"VK_SNAPSHOT",
	"VK_INSERT",
	"VK_DELETE",
	"VK_HELP",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"VK_LWIN",
	"VK_RWIN",
	"VK_APPS",
	"Unknown",
	"VK_SLEEP",
	"VK_NUMPAD0",
	"VK_NUMPAD1",
	"VK_NUMPAD2",
	"VK_NUMPAD3",
	"VK_NUMPAD4",
	"VK_NUMPAD5",
	"VK_NUMPAD6",
	"VK_NUMPAD7",
	"VK_NUMPAD8",
	"VK_NUMPAD9",
	"VK_MULTIPLY",
	"VK_ADD",
	"VK_SEPARATOR",
	"VK_SUBTRACT",
	"VK_DECIMAL",
	"VK_DIVIDE",
	"VK_F1",
	"VK_F2",
	"VK_F3",
	"VK_F4",
	"VK_F5",
	"VK_F6",
	"VK_F7",
	"VK_F8",
	"VK_F9",
	"VK_F10",
	"VK_F11",
	"VK_F12",
	"VK_F13",
	"VK_F14",
	"VK_F15",
	"VK_F16",
	"VK_F17",
	"VK_F18",
	"VK_F19",
	"VK_F20",
	"VK_F21",
	"VK_F22",
	"VK_F23",
	"VK_F24",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"VK_NUMLOCK",
	"VK_SCROLL",
	"VK_OEM_NEC_EQUAL",
	"VK_OEM_FJ_MASSHOU",
	"VK_OEM_FJ_TOUROKU",
	"VK_OEM_FJ_LOYA",
	"VK_OEM_FJ_ROYA",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"VK_LSHIFT",
	"VK_RSHIFT",
	"VK_LCONTROL",
	"VK_RCONTROL",
	"VK_LMENU",
	"VK_RMENU"
};

struct ImGuiItem
{
	enum type
	{
		frame = 0,
		text
	};

	ImVec2 pos1;
	ImVec2 pos2;
	ImU32 color;
	bool border;
	float rounding;
	const char* str;
	type item_type;
};

std::vector<ImGuiItem> items_to_render;

bool ImGui::Hotkey(const char* label, c_config::pbind k)
{
	ImVec2 size_arg = ImVec2(150, 24);
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	ImGuiIO& io = g.IO;
	const ImGuiStyle& style = g.Style;

	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
	ImVec2 size = ImGui::CalcItemSize(size_arg, ImGui::CalcItemWidth(), label_size.y + style.FramePadding.y * 2.0f);
	const ImRect frame_bb(window->DC.CursorPos + ImVec2(label_size.x + style.ItemInnerSpacing.x, 0.0f), window->DC.CursorPos + size);
	const ImRect total_bb(window->DC.CursorPos, frame_bb.Max);

	ImGui::ItemSize(total_bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(total_bb, id))
		return false;

	const bool focus_requested = ImGui::FocusableItemRegister(window, /*g.ActiveId ==*/ id/*, false*/);
	//const bool focus_requested_by_code = focus_requested && (window->DC.FocusCounterRegular == window->DC.FocusIdxAllRequestCurrent);
	//const bool focus_requested_by_tab = focus_requested && !focus_requested_by_code;

	const bool hovered = ImGui::ItemHoverable(frame_bb, id);

	if (hovered) {
		ImGui::SetHoveredID(id);
		g.MouseCursor = ImGuiMouseCursor_TextInput;
	}

	const bool user_clicked = hovered && io.MouseClicked[0];

	if ((focus_requested || user_clicked) && !k->opened_parameters) {
		if (g.ActiveId != id) {
			// Start edition
			memset(io.MouseDown, 0, sizeof(io.MouseDown));
			memset(io.KeysDown, 0, sizeof(io.KeysDown));
			k->key = 0;
		}
		ImGui::SetActiveID(id, window);
		ImGui::FocusWindow(window);
	}
	else if (io.MouseClicked[0]) {
		// Release focus when we click outside
		if (g.ActiveId == id)
			ImGui::ClearActiveID();
	}

	bool value_changed = false;
	int key = k->key;

	if (g.ActiveId == id) {
		for (auto i = 0; i < 5; i++) {
			if (io.MouseDown[i]) {
				switch (i) {
				case 0:
					key = VK_LBUTTON;
					break;
				case 1:
					key = VK_RBUTTON;
					break;
				case 2:
					key = VK_MBUTTON;
					break;
				case 3:
					key = VK_XBUTTON1;
					break;
				case 4:
					key = VK_XBUTTON2;
					break;
				}
				value_changed = true;
				ImGui::ClearActiveID();
			}
		}
		if (!value_changed) {
			for (auto i = VK_BACK; i <= VK_RMENU; i++) {
				if (io.KeysDown[i]) {
					key = i;
					value_changed = true;
					ImGui::ClearActiveID();
				}
			}
		}

		if (IsKeyPressedMap(ImGuiKey_Escape)) {
			k->key = 0;
			ImGui::ClearActiveID();
		}
		else {
			k->key = key;
		}
	}

	// Render
	// Select which buffer we are going to display. When ImGuiInputTextFlags_NoLiveEdit is Set 'buf' might still be the old value. We Set buf to NULL to prevent accidental usage from now on.

	char buf_display[64] = "None";

	ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImVec4(0.20f, 0.25f, 0.30f, 1.0f)), true, style.FrameRounding);

	if (k->key != 0 && g.ActiveId != id) {
		strcpy_s(buf_display, KeyNames[k->key]);
	}
	else if (g.ActiveId == id) {
		strcpy_s(buf_display, "<Press a key>");
	}

	const ImRect clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + size.x, frame_bb.Min.y + size.y); // Not using frame_bb.Max because we have adjusted size
	ImVec2 render_pos = ImVec2(frame_bb.Min.x + style.FramePadding.x, frame_bb.Min.y + style.FramePadding.y);
	ImGui::RenderTextClipped(ImVec2(frame_bb.Min.x + style.FramePadding.x, frame_bb.Min.y + style.FramePadding.y), ImVec2(frame_bb.Max.x - style.FramePadding.x, frame_bb.Max.y - style.FramePadding.y), buf_display, NULL, NULL, style.ButtonTextAlign, &clip_rect);

	if (label_size.x > 0)
		ImGui::RenderText(ImVec2(total_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), label);

	if (!value_changed && hovered && io.MouseDown[1] && g.ActiveId != id || k->opened_parameters)
	{
		k->opened_parameters = true;

		items_to_render.push_back({ ImVec2(frame_bb.Max.x - 20, frame_bb.Max.y - 20), ImVec2(frame_bb.Max.x + 80, frame_bb.Max.y + 30), ImGui::GetColorU32(ImVec4(0, 0, 0, 1.0f)), true, style.FrameRounding, 0, ImGuiItem::type::frame });

		ImU32 inactive = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		ImU32 active = ImGui::GetColorU32(ImVec4(0, 1.0f, 0.0f, 1.0f));

		ImVec2 pos1 = ImVec2(frame_bb.Max.x - 15, frame_bb.Max.y - 15);
		ImVec2 pos2 = pos1;
		pos2.y += ImGui::CalcTextSize("Hold").y + 3;

		items_to_render.push_back({ pos1, ImVec2(), (k->mode == 0 ? active : inactive), 0, 0, "Hold", ImGuiItem::type::text });
		items_to_render.push_back({ pos2, ImVec2(), (k->mode == 1 ? active : inactive), 0, 0, "Toggle", ImGuiItem::type::text });

		if (io.MousePos.x > frame_bb.Max.x - 20 && io.MousePos.y > frame_bb.Max.y - 20 &&
			io.MousePos.x < frame_bb.Max.x + 80 && io.MousePos.y < frame_bb.Max.y + 30 && io.MouseDown[0])
		{
			if (io.MousePos.y < pos2.y)
				k->mode = 0;
			else k->mode = 1;

			k->opened_parameters = false;
		}
				
		if (IsKeyPressedMap(ImGuiKey_Escape) || !hovered && io.MouseDown[0])
			k->opened_parameters = false;
	}

	return value_changed;
}

char to_lower(char c)
{
	if (c == 'À') return 'à';
	if (c == 'Á') return 'á';
	if (c == 'Â') return 'â';
	if (c == 'Ã') return 'ã';
	if (c == 'Ä') return 'ä';
	if (c == 'Å') return 'å';
	if (c == '¨') return '¸';
	if (c == 'Æ') return 'æ';
	if (c == 'Ç') return 'ç';
	if (c == 'È') return 'è';
	if (c == 'Ê') return 'ê';
	if (c == 'Ë') return 'ë';
	if (c == 'Ì') return 'ì';
	if (c == 'Í') return 'í';
	if (c == 'Î') return 'î';
	if (c == 'Ï') return 'ï';
	if (c == 'Ð') return 'ð';
	if (c == 'Ñ') return 'ñ';
	if (c == 'Ò') return 'ò';
	if (c == 'Ó') return 'ó';
	if (c == 'Ô') return 'ô';
	if (c == 'Õ') return 'õ';
	if (c == 'Ö') return 'ö';
	if (c == '×') return '÷';
	if (c == 'Ø') return 'ø';
	if (c == 'Ù') return 'ù';
	if (c == 'Ú') return 'ú';
	if (c == 'Û') return 'û';
	if (c == 'Ü') return 'ü';
	if (c == 'Ý') return 'ý';
	if (c == 'Þ') return 'þ';
	if (c == 'ß') return 'ÿ';
}

void c_menu::draw() 
{
	if (!menu_opened) return;

	items_to_render.clear();
	
	//ImGui::ShowStyleEditor();

	ImGui::SetNextWindowSize(ImVec2(1047, 527));
	ImGui::Begin("KatzeCheats Free", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);

	static int CWindow = 0;
	
	//start
	{
		ImGui::PushFont(Font25);

		ImGui::GetStyle().ButtonTextAlign.y = 0.60f;

		if (CWindow == 6) active_tab(); else inactive_tab();
		ImGui::SetCursorPos(ImVec2(20, 24));
		if (ImGui::Button("RAGE", ImVec2(94, 50))) CWindow = 6;

		if (CWindow == 1) active_tab(); else inactive_tab();
		ImGui::SetCursorPos(ImVec2(143, 24));
		if (ImGui::Button("LEGIT", ImVec2(94, 50))) CWindow = 1;

		if (CWindow == 2) active_tab(); else inactive_tab();
		ImGui::SetCursorPos(ImVec2(266, 24));
		if (ImGui::Button("TRIGGERBOT", ImVec2(188, 50))) CWindow = 2;

		if (CWindow == 3) active_tab(); else inactive_tab();
		ImGui::SetCursorPos(ImVec2(483, 24));
		if (ImGui::Button("VISUALS", ImVec2(139, 50))) CWindow = 3;

		if (CWindow == 4) active_tab(); else inactive_tab();
		ImGui::SetCursorPos(ImVec2(651, 24));
		if (ImGui::Button("SKINS", ImVec2(97, 50))) CWindow = 4;

		if (CWindow == 5) active_tab(); else inactive_tab();
		ImGui::SetCursorPos(ImVec2(777, 24));
		if (ImGui::Button("MISC", ImVec2(100, 50))) CWindow = 5;

		reset_buttons();

		ImGui::GetStyle().ButtonTextAlign.y = 0.50f;

		ImGui::PopFont();		

		ImGui::SetCursorPos(ImVec2(0, 97));
		ImGui::Separator();
	}

	//main
	{
		if (CWindow == Aimbot)
		{
			ImGui::Columns(3, 0, false);
			
			ImGui::Checkbox("Enabled", &c_config::legit::legit_aimbot);
			ImGui::Checkbox("Disable if you are flashed", &c_config::legit::disable_if_flashed);
			ImGui::Checkbox("Disable if enemy is in smoke", &c_config::legit::disable_in_smoke);
			ImGui::Checkbox("Check if visible", &c_config::legit::visible_check);
			ImGui::Checkbox("Attack teammates", &c_config::legit::attack_teammates);
			ImGui::Text("Autofire"); ImGui::SameLine(); ImGui::Hotkey("##autofire", &c_config::legit::autofire);
			ImGui::Checkbox("Autostop", &c_config::legit::autostop);

			ImGui::NextColumn();

			ImGui::PushItemWidth(335.0f);

			ImGui::Combo("##currentweapon", &cw, "Not selected\0Glock-18\0P2000\0USP-S\0Dual Berettas\0P250\0Tec-9\0Five-Seven\0CZ-75\0Desert Eagle\0Revolver\0Nova\0XM1014\0Sawed-off\0MAG-7\0M249\0Negev\0Mac-10\0MP9\0MP7\0MP5-SD\0UMP-45\0P90\0PP-Bizon\0Galil AR\0Famas\0AK-47\0M4A4\0M4A1-S\0SSG-08\0SG-553\0AUG\0AWP\0G3SG1\0SCAR-20\0");
			if (ImGui::Checkbox("Auto", &c_config::legit::auto_pick))
			{
				c_base_entity* local = interfaces::client_ent_list->get_client_entity(interfaces::engine_client->get_local_player());
				if (local && c_config::legit::auto_pick)
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

			ImGui::Combo("##hitbox", &c_config::legit::weapons[cw].hitbox, "Nearest\0Head\0Neck\0Sternum\0Chest\0Stomach\0Pelvis\0");

			if (cw <= 9 && cw) ImGui::Checkbox("Autopistol", &c_config::legit::weapons[cw].autopistol);
			ImGui::Checkbox("Backtrack", &c_config::legit::weapons[cw].legit_backtrack);
			if (cw == 32 || cw == 29 || cw == 33 || cw == 34)
				ImGui::Checkbox("Only in scope", &c_config::legit::weapons[cw].only_scoped);
			ImGui::SliderFloat("##fov", &c_config::legit::weapons[cw].fov, 0.0f, 90.0f, "FOV: %.2f");
			ImGui::SliderFloat("##silent_fov", &c_config::legit::weapons[cw].silent_fov, 0.0f, 30.0f, "Silent FOV: %.2f");
			ImGui::SliderFloat("##smooth", &c_config::legit::weapons[cw].smooth, 1.0f, 20.0f, "Smooth: %.2f");
			ImGui::SliderInt("##killdelay", &c_config::legit::weapons[cw].kill_delay, 0, 1000, "Kill delay: %d ms");
			ImGui::SliderInt("##hitchance", &c_config::legit::weapons[cw].hitchance, 0, 100, "Hitchance: %d");

			ImGui::PopItemWidth();

			ImGui::NextColumn();

			if (cw > 14 && cw != 32 && cw != 29)
			{
				ImGui::PushItemWidth(335.0f);
				ImGui::SliderFloat("##rcsx", &c_config::legit::weapons[cw].rcs_x, 0.0f, 1.0f, "RCS X: %.2f");
				ImGui::SliderFloat("##rcsy", &c_config::legit::weapons[cw].rcs_y, 0.0f, 1.0f, "RCS Y: %.2f");
				ImGui::SliderFloat("##rcsfov", &c_config::legit::weapons[cw].rcs_fov, 0.0f, 90.0f, "RCS FOV: %.2f");
				ImGui::SliderFloat("##rcssmooth", &c_config::legit::weapons[cw].rcs_smooth, 1.0f, 20.0f, "RCS Smooth: %.2f");
				ImGui::SliderInt("##rcsstart", &c_config::legit::weapons[cw].rcs_start_bullet, 2, 30, "RCS Start: %d");
				ImGui::Checkbox("RCS Standalone", &c_config::legit::weapons[cw].rcs_standalone);
				ImGui::PopItemWidth();
			}

			ImGui::NextColumn();
		}

		if (CWindow == Triggerbot)
		{
			ImGui::Columns(2, 0, 0);

			ImGui::Checkbox("Disable in smoke", &c_config::triggerbot::disable_in_smoke);
			ImGui::Checkbox("Disable if flashed", &c_config::triggerbot::disable_if_flashed);
			ImGui::Combo("##currentweapon", &cw, "Not selected\0Glock-18\0P2000\0USP-S\0Dual Berettas\0P250\0Tec-9\0Five-Seven\0CZ-75\0Desert Eagle\0Revolver\0Nova\0XM1014\0Sawed-off\0MAG-7\0M249\0Negev\0Mac-10\0MP9\0MP7\0MP5-SD\0UMP-45\0P90\0PP-Bizon\0Galil AR\0Famas\0AK-47\0M4A4\0M4A1-S\0SSG-08\0SG-553\0AUG\0AWP\0G3SG1\0SCAR-20\0");
			if (ImGui::Checkbox("Auto", &c_config::legit::auto_pick))
			{
				c_base_entity* local = interfaces::client_ent_list->get_client_entity(interfaces::engine_client->get_local_player());
				if (local && c_config::legit::auto_pick)
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

			ImGui::Checkbox("Enabled", &c_config::triggerbot::weapons[cw].enabled);
			ImGui::SameLine();
			ImGui::Hotkey("##triggerbotkey", &c_config::triggerbot::key);

			if (cw == 29 || cw == 32 || cw == 33 || cw == 34)
				ImGui::Checkbox("Only in scope", &c_config::triggerbot::weapons[cw].only_scoped);

			ImGui::SliderInt("##hitchance", &c_config::triggerbot::weapons[cw].hitchance, 0, 100, "Hitchance: %d");

			ImGui::NextColumn();

			ImGui::ListBoxHeader("Hitgroups");

			ImGui::Selectable("Head", &c_config::triggerbot::weapons[cw].hitboxes[0]);
			ImGui::Selectable("Chest", &c_config::triggerbot::weapons[cw].hitboxes[1]);
			ImGui::Selectable("Stomach", &c_config::triggerbot::weapons[cw].hitboxes[2]);
			ImGui::Selectable("Left arm", &c_config::triggerbot::weapons[cw].hitboxes[3]);
			ImGui::Selectable("Right arm", &c_config::triggerbot::weapons[cw].hitboxes[4]);
			ImGui::Selectable("Left leg", &c_config::triggerbot::weapons[cw].hitboxes[5]);
			ImGui::Selectable("Right leg", &c_config::triggerbot::weapons[cw].hitboxes[6]);

			ImGui::ListBoxFooter();

			ImGui::NextColumn();
		}

		if (CWindow == Visuals)
		{
			ImGui::Columns(3, 0, 0);

			ImGui::PushItemWidth(335.0f);

			ImGui::Checkbox("Teammates##boxes", &c_config::visuals::esp_teammates);
			ImGui::ColorPicker("##boxcolor", &c_config::visuals::esp_box_colour); ImGui::SameLine();
			ImGui::Checkbox("Boxes", &c_config::visuals::esp_boxes); 
			ImGui::Checkbox("Health", &c_config::visuals::esp_health);
			ImGui::Checkbox("Health bar", &c_config::visuals::esp_health_bar);
			ImGui::Checkbox("Armor", &c_config::visuals::esp_armour);
			ImGui::Checkbox("Names", &c_config::visuals::esp_names);
			ImGui::Checkbox("Weapon", &c_config::visuals::esp_weapon);

			ImGui::PopItemWidth();

			ImGui::NextColumn();

			ImGui::PushItemWidth(335.0f);

			static int chams_type = 0;

			ImGui::Combo("##chamstype", &chams_type, "Players\0Arms\0Weapons\0");

			if (chams_type == 0)
			{
				ImGui::Checkbox("Chams", &c_config::visuals::chams);
				ImGui::Checkbox("Teammates##chams", &c_config::visuals::chams_teammates);
				ImGui::Checkbox("Ignore-Z", &c_config::visuals::chams_ignore_z);
				ImGui::Checkbox("Visualize backtrack", &c_config::visuals::chams_backtrack);
				ImGui::Combo("##material", &c_config::visuals::chams_type, "Normal\0Crystal\0Glow\0");

				ImGui::ColorPicker("Visible teammates", &c_config::visuals::chams_colour_vis_t);
				ImGui::ColorPicker("Invisible teammates", &c_config::visuals::chams_colour_invis_t);
				ImGui::ColorPicker("Visible enemies", &c_config::visuals::chams_colour_vis_e);
				ImGui::ColorPicker("Invisible enemies", &c_config::visuals::chams_colour_invis_e);
				ImGui::ColorPicker("Backtrack", &c_config::visuals::chams_backtrack_color);
			}
			
			if (chams_type == 1)
			{
				ImGui::ColorPicker("##armscolor", &c_config::visuals::chams_arms_color);
				ImGui::SameLine();
				ImGui::Checkbox("Enabled", &c_config::visuals::chams_arms);	
				ImGui::Checkbox("Wireframe", &c_config::visuals::chams_arms_wireframe);	
				ImGui::SliderFloat("##alpha", &c_config::visuals::chams_arms_alpha, 0.0f, 1.0f, "Alpha: %.2f");
				ImGui::Combo("##material", &c_config::visuals::chams_arms_type, "Normal\0Metallic\0Glow\0");
			}

			if (chams_type == 2)
			{
				ImGui::ColorPicker("##armscolor", &c_config::visuals::chams_weapons_color);
				ImGui::SameLine();
				ImGui::Checkbox("Enabled", &c_config::visuals::chams_weapons);
				ImGui::Checkbox("Wireframe", &c_config::visuals::chams_weapons_wireframe);
				ImGui::SliderFloat("##alpha", &c_config::visuals::chams_weapons_alpha, 0.0f, 1.0f, "Alpha: %.2f");
				ImGui::Combo("##material", &c_config::visuals::chams_weapons_type, "Normal\0Metallic\0Glow\0");
			}

			ImGui::PopItemWidth();

			ImGui::NextColumn();

			ImGui::Checkbox("Show keybinds", &c_config::visuals::show_binds);
			ImGui::Checkbox("Angle lines", &c_config::visuals::angle_lines);
			ImGui::Checkbox("Wireframe smoke", &c_config::visuals::wireframe_smoke);
			ImGui::Checkbox("Grenade Predict", &c_config::visuals::nade_predict);
			ImGui::Checkbox("Nightmode", &c_config::visuals::nightmode);
			ImGui::Checkbox("Reveal ranks", &c_config::visuals::reveal_ranks);
			ImGui::Checkbox("Show FOV", &c_config::visuals::show_fov);
			ImGui::Checkbox("Remove scope", &c_config::visuals::remove_scope);
			ImGui::SliderInt("##viewmodelfov", &c_config::visuals::viewmodel_fov, 0, 90, "Viewmodel FOV: %d");

			ImGui::NextColumn();
		}

		if (CWindow == Skins)
		{
			ImGui::Combo("##currentweapon", &cw, "Knife\0Glock-18\0P2000\0USP-S\0Dual Berettas\0P250\0Tec-9\0Five-Seven\0CZ-75\0Desert Eagle\0Revolver\0Nova\0XM1014\0Sawed-off\0MAG-7\0M249\0Negev\0Mac-10\0MP9\0MP7\0MP5-SD\0UMP-45\0P90\0PP-Bizon\0Galil AR\0Famas\0AK-47\0M4A4\0M4A1-S\0SSG-08\0SG-553\0AUG\0AWP\0G3SG1\0SCAR-20\0");
			if (ImGui::Checkbox("Auto", &c_config::legit::auto_pick))
			{
				c_base_entity* local = interfaces::client_ent_list->get_client_entity(interfaces::engine_client->get_local_player());
				if (local && c_config::legit::auto_pick)
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
			
			bool update = false;

			if (!cw)
				ImGui::Combo("Knife", &c_config::skins::knife_id, "-\0Butterfly\0Canis\0Cord\0Classic\0Falchion\0Flip\0Gut\0Gypsy\0Karambit\0M9 Bayonet\0Outdoor\0Shadow daggers\0Skeleton\0Stiletto\0Bowie\0Huntsman\0Ursus\0Talon\0Bayonet\0");

			static char input_paint_kit[32];
			ImGui::InputText("Search", input_paint_kit, IM_ARRAYSIZE(input_paint_kit));

			static std::vector<int> vec;
			vec.clear();
			static std::vector<std::string> strings;
			strings.clear();
			static int cid = 0;

			if (cw)
			{
				for (int i = 0; i < features::skins->skin_kits.size(); i++)
				{
					if (features::skins->skin_kits[c_config::skins::skins[cw].id].id == features::skins->skin_kits[i].id)
					{
						cid = vec.size();
						strings.push_back(features::skins->skin_kits[i].name);
						vec.push_back(i);
						continue;
					}

					std::string s1 = features::skins->skin_kits[i].name;
					std::string s2 = input_paint_kit;

					std::transform(s1.begin(), s1.end(), s1.begin(),
						[](unsigned char c) { return std::tolower(c); });
					std::transform(s2.begin(), s2.end(), s2.begin(),
						[](unsigned char c) { return std::tolower(c); });

					if (s1.find(s2) != std::string::npos || !strlen(input_paint_kit))
					{
						strings.push_back(features::skins->skin_kits[i].name);
						vec.push_back(i);
					}
				}

				update = ImGui::Combo("Paint kit", &cid, [](void* data, int idx, const char** out_text) {
					*out_text = strings[idx].c_str();
					return true;
					}, nullptr, strings.size());

				c_config::skins::skins[cw].id = vec[cid];
			}
			else
			{
				for (int i = 0; i < features::skins->skin_kits.size(); i++)
				{
					if (features::skins->skin_kits[c_config::skins::knife.id].id == features::skins->skin_kits[i].id)
					{
						cid = vec.size();
						strings.push_back(features::skins->skin_kits[i].name);
						vec.push_back(i);
						continue;
					}

					std::string s1 = features::skins->skin_kits[i].name;
					std::string s2 = input_paint_kit;

					std::transform(s1.begin(), s1.end(), s1.begin(),
						[](unsigned char c) { return std::tolower(c); });
					std::transform(s2.begin(), s2.end(), s2.begin(),
						[](unsigned char c) { return std::tolower(c); });

					if (s1.find(s2) != std::string::npos || !strlen(input_paint_kit))
					{
						strings.push_back(features::skins->skin_kits[i].name);
						vec.push_back(i);
					}
				}

				update = ImGui::Combo("Paint kit", &cid, [](void* data, int idx, const char** out_text) {
					*out_text = strings[idx].c_str();
					return true;
					}, nullptr, strings.size());

				c_config::skins::knife.id = vec[cid];
			}

			ImGui::SliderFloat("Wear", cw ? &c_config::skins::skins[cw].wear : &c_config::skins::knife.wear, 0.00001f, 1.0f, "%.5f");
			
			/*if (ImGui::Button("Update", { ImGui::GetCurrentWindow()->DC.ItemWidth, 24 }))
				update = true;*/

			if (update)
				(*interfaces::client_state)->force_full_update();
		}

		if (CWindow == Misc)
		{
			ImGui::Columns(2, 0, 0);
			ImGui::Checkbox("Bunnyhop", &c_config::misc::bunnyhop);
			ImGui::Checkbox("Autostrafe", &c_config::misc::autostrafe);
			ImGui::Checkbox("Clantag", &c_config::misc::clantag_changer);
			ImGui::Checkbox("Fast duck", &c_config::misc::infinity_duck);
			ImGui::Checkbox("Slowwalk", &c_config::misc::slowwalk);
			if (c_config::misc::slowwalk)
				ImGui::SliderFloat("##speed", &c_config::misc::slowwalk_speed, 0.0f, 100.0f, "Speed: %.2f%%");
			ImGui::Checkbox("Legit Resolver", &c_config::misc::legit_resolver);
			ImGui::Checkbox("Bypass sv_pure", &c_config::misc::svpure_bypass);
			ImGui::Checkbox("Legit AA", &c_config::misc::legit_aa); ImGui::SameLine(); ImGui::Hotkey("##legitaa", &c_config::misc::legit_aa_inverter);
			c_config::visuals::thirdperson = c_config::visuals::thirdperson_key.state;
			if(ImGui::Checkbox("Thirdperson", &c_config::visuals::thirdperson)) c_config::visuals::thirdperson_key.state = c_config::visuals::thirdperson; 
			ImGui::SameLine();
			ImGui::Hotkey("##3rdpersonbind", &c_config::visuals::thirdperson_key);
			ImGui::SliderInt("##thirdpersondist", &c_config::visuals::thirdperson_dist, 0, 300, "Distance: %d");

			ImGui::NextColumn();

			static int cc = 0;
			ImGui::Combo("Configs", &cc, [](void* data, int idx, const char** out_text) {
				*out_text = c_config::configs[idx].c_str();
				return true;
				}, nullptr, c_config::configs.size());

			if (ImGui::Button("Save"))
				c_config::save(cc);
			ImGui::SameLine();
			if (ImGui::Button("Load"))
				c_config::load(cc);
			ImGui::SameLine();
			if (ImGui::Button("Delete"))
				c_config::del(cc);
			ImGui::SameLine();
			if (ImGui::Button("Update"))
				c_config::init();

			static char newcfg[32];
			ImGui::InputText("Name", newcfg, IM_ARRAYSIZE(newcfg));
			if (ImGui::Button("Create"))
				c_config::create(newcfg);

			ImGui::NextColumn();
		}

		if (CWindow == Ragebot)
		{
			ImGui::Columns(3, 0, 0);

			ImGui::Checkbox("Enabled", &c_config::rage::enabled);
			ImGui::Checkbox("Autostop", &c_config::rage::autostop);
			ImGui::Checkbox("Autoscope", &c_config::rage::autoscope);
			ImGui::Checkbox("Autofire", &c_config::rage::autofire);
			ImGui::Checkbox("Bruteforce", &c_config::rage::bruteforce);

			ImGui::NextColumn();

			ImGui::Checkbox("Antiaims", &c_config::rage::antiaims_enabled);
			ImGui::Checkbox("Desync", &c_config::rage::desync);

			static int c = 0;
			ImGui::Combo("Type", &c, "Standing\0Moving\0In air\0");
			ImGui::Combo("Pitch", &c_config::rage::antiaims[c].pitch, "None\0Down\0Up\0");
			ImGui::Combo("Yaw", &c_config::rage::antiaims[c].yaw, "None\0Backward\0Manual\0Random\0");

			ImGui::Hotkey("##ManualBack", &c_config::rage::manual_back); ImGui::SameLine(); ImGui::Text("Back");
			ImGui::Hotkey("##ManualLeft", &c_config::rage::manual_left); ImGui::SameLine(); ImGui::Text("Left");
			ImGui::Hotkey("##ManualRight", &c_config::rage::manual_right); ImGui::SameLine(); ImGui::Text("Right");

			ImGui::SliderInt("##fakelag", &c_config::rage::fakelag, 0, 15, "Fakelag: %d");

			ImGui::NextColumn();

			static int weapon = 0;
			ImGui::Combo("##weapon", &weapon, "Pistols\0Heavy pistols\0SSG\0AWP\0Auto\0Others\0");
			ImGui::SliderInt("##hc", &c_config::rage::weapons[weapon].hitchance, 0, 100, "Hitchance: %d");
			ImGui::SliderInt("##dmg", &c_config::rage::weapons[weapon].mindmg, 0, 100, "Min damage: %d");

			if (ImGui::BeginCombo("##hitboxes", "Hitboxes", ImGuiComboFlags_PopupAlignLeft))
			{
				static const char* hitboxes[] = {
				"Head",
				"Neck",
				"Body",
				"Arms",
				"Legs"
				};

				for (int i = 0; i < 5; i++)
					ImGui::Selectable(hitboxes[i], &c_config::rage::weapons[weapon].hitboxes[i]);

				ImGui::EndCombo();
			}

			ImGui::SliderInt("##headscale", &c_config::rage::weapons[weapon].head_scale, 0, 100, "Head scale: %d");
			ImGui::SliderInt("##bodyscale", &c_config::rage::weapons[weapon].body_scale, 0, 100, "Body scale: %d");

			ImGui::NextColumn();
		}
	}

	//end
	{
		ImGui::SetCursorPos(ImVec2(0, 457));
		ImGui::Separator();

		ImGui::PushFont(Font27);

		static int phase = 0;
		static int pos = 0;

		if (phase % 15 == 0) pos++;
		pos = pos % 4;
		phase++;
		if (phase >= INT_MAX - 1) phase = 0;

		switch (pos)
		{
		case 0:
			ImGui::SetCursorPos(ImVec2(21, 480));
			ImGui::TextColored(ImVec4(1, 0, 1, 1), "KATZECHEATS");
			break;
		case 1:
		case 3: break;
		case 2:
			ImGui::SetCursorPos(ImVec2(17, 480));
			ImGui::TextColored(ImVec4(0, 1, 1, 1), "KATZECHEATS");
			break;
		}

		ImGui::SetCursorPos(ImVec2(19, 480));
		ImGui::Text("KATZECHEATS");

		ImGui::PopFont();

		ImGui::SetCursorPos(ImVec2(225, 487));
		ImGui::Text("FREE");
	}

	for (auto i : items_to_render)
		if (i.item_type == ImGuiItem::type::frame)
			ImGui::RenderFrame(i.pos1, i.pos2, i.color, i.border, i.rounding);
		else if (i.item_type == ImGuiItem::type::text)
			ImGui::GetWindowDrawList()->AddText(i.pos1, i.color, i.str);

	ImGui::End();
}