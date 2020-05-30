#include "render_manager.h"

#include "imgui/imgui.h"
#include "NotoSans.h"

namespace render_manager {
	namespace fonts {
		ImFont* esp_font;
	}

	ImDrawList* draw_list;
}

void render_manager::init() {
	global_utils::console_log("initialising draw manager...");

	fonts::esp_font = ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)notosans_data, notosans_size, 20, 0, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

	global_utils::console_log("draw manager initialised...");
}

void render_manager::rect(int x, int y, int w, int h, color color) {
	draw_list->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f)));
}

void render_manager::filled_rect(int x, int y, int w, int h, color color) {
	draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f)));
}

void render_manager::vertical_gradient_rect(int x, int y, int w, int h, color top, color bottom) { // no
	/*interfaces::surface->draw_set_color(top.r, top.g, top.b, top.a);
	interfaces::surface->draw_filled_rect_fade(x, y, w, h, 255, 255, false);
	interfaces::surface->draw_set_color(bottom.r, bottom.g, bottom.b, bottom.a);
	interfaces::surface->draw_filled_rect_fade(x, y, w, h, 0, 255, false);*/
}

void render_manager::horizontal_gradient_rect(int x, int y, int w, int h, color left, color right) { // no
	/*interfaces::surface->draw_set_color(left.r, left.g, left.b, left.a);
	interfaces::surface->draw_filled_rect_fade(x, y, w, h, 255, 255, true);
	interfaces::surface->draw_set_color(right.r, right.g, right.b, right.a);
	interfaces::surface->draw_filled_rect_fade(x, y, w, h, 0, 255, true);*/
}

void render_manager::text(const char* text, int x, int y, bool centered, color color) {

	ImVec2 pos = ImVec2(x, y);
	if (centered)
		pos.x -= (ImGui::CalcTextSize(text).x / 2.0f);

	draw_list->AddText(ImVec2(pos.x + 1, pos.y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text);
	draw_list->AddText(ImVec2(pos.x - 1, pos.y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text);
	draw_list->AddText(ImVec2(pos.x + 1, pos.y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text);
	draw_list->AddText(ImVec2(pos.x - 1, pos.y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text);

	draw_list->AddText(pos, ImGui::ColorConvertFloat4ToU32(ImVec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, 1)), text);
}
