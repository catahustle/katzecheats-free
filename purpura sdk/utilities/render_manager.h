#pragma once

#include "../interfaces/interfaces.h"

#include "imgui/imgui.h"

namespace render_manager {
	namespace fonts {
		extern ImFont* esp_font;
	}

	extern ImDrawList* draw_list;
	void init();
	void rect(int x, int y, int w, int h, color color);
	void filled_rect(int x, int y, int w, int h, color color);
	void vertical_gradient_rect(int x, int y, int w, int h, color top, color bottom);
	void horizontal_gradient_rect(int x, int y, int w, int h, color left, color right);
	void text(const char *text, int x, int y, bool centered, color color);
}