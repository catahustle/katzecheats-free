#pragma once

#include "../../interfaces/interfaces.h"
#include "../../sdk/c_input.h"
#include "../../sdk/i_client_entity.h"
#include "../../hooks/hooks.h"

namespace features {
	class visuals {
	public:
		struct esp_box {
			int x, y, w, h;
		};

		void render();
		bool render_chams(hooks::draw_model_execute_t, int, i_mat_render_context*, const draw_model_state_t&, const model_render_info_t&, matrix3x4_t*);
		void thirdperson();
		void modify_smoke();
		void nightmode();
	private:
		const bool calculate_box_position(c_base_entity * entity, features::visuals::esp_box & box);
		void render_box(esp_box box);
		void render_health_bar(esp_box box, int entity_index);
		void render_name(esp_box box, int entity_index);
		void render_money(esp_box box, int entity_index);
		void render_weapon(esp_box box, int entity_index);
		void render_health(esp_box box, int entity_index);
		void render_armour(esp_box box, int entity_index);
		void render_lines(c_base_entity* local);
		void render_binds();
		void render_fovs(c_base_entity*);

		RECT get_text_size(DWORD font, const char* text, ...) {
			std::string text_normal = text;
			std::wstring text_wide = std::wstring(text_normal.begin(), text_normal.end());
			const wchar_t* final_text = text_wide.c_str();

			RECT rect; int x, y;
			interfaces::surface->get_text_size(font, final_text, x, y);
			rect.left = x; rect.bottom = y;
			rect.right = x;
			return rect;
		}
	};

	extern visuals* vis;
}
