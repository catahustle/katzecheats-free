#pragma once

#include <vector>
#include <memory>
#include <random>
#include <iostream>
#include <ShlObj_core.h>
#include <functional>
#include "..\utilities\render_manager.h"
#include "..\utilities\imgui\imgui.h"
#include "settings.h"

namespace ImGui
{
	bool Hotkey(const char* label, c_config::pbind k);
}

class c_menu {
public:
	bool menu_opened;
	void draw();
	void setup_styles();

private:
	enum Tabs {
		Aimbot = 1,
		Triggerbot,
		Visuals,
		Skins,
		Misc,
		Ragebot
	};

	ImFont* Font;
	ImFont* Font25;
	ImFont* Font27;
	
	void active_tab();
	void inactive_tab();
	void reset_buttons();
};

extern c_menu menu;
