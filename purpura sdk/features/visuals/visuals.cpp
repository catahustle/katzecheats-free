#include "visuals.h"
#include "../legit/legit_lag_comp.h"

#include "../../sdk/iv_engine_client.h"
#include "../../sdk/entities.h"

#include "../../utilities/render_manager.h"
#include "../../utilities/math.h"
#include "../../utilities/globals.h"

#include "../../menu/settings.h"

#include "../../utilities/imgui/imgui.h"

#include "../../hooks/hooks.h"

#include <algorithm>

int y_position_displace = 1;

const bool features::visuals::calculate_box_position(c_base_entity* entity, features::visuals::esp_box &box) {
	vec3_t top, down, s[2], air;

	vec3_t adjust = vec3_t(0, 0, -15 * entity->crouch_ammount());

	down = entity->abs_origin();

	if (!(entity->flags() & fl_onground) && (entity->move_type() != move_types::ladder))
		air = vec3_t(0, 0, 10);
	else
		air = vec3_t(0, 0, 0);

	down = entity->abs_origin() + air;

	if ((entity->flags() & fl_ducking))
	{
		top = down + vec3_t(0, 0, 58) + adjust;
	}
	else {
		top = down + vec3_t(0, 0, 72) + adjust;
	}

	if (global_utils::world_to_screen(top, s[1]) && global_utils::world_to_screen(down, s[0]))
	{
		vec3_t delta = s[1] - s[0];

		box.h = static_cast<int>(fabsf(delta.y) + 6);
		box.w = static_cast<int>(box.h / 2 + 5);

		box.x = static_cast<int>(s[1].x - (box.w / 2) + 2);
		box.y = static_cast<int>(s[1].y - 1);

		return true;
	}

	return false;
}
void features::visuals::render_box(esp_box box) {
	render_manager::rect(box.x, box.y, box.w, box.h, c_config::visuals::esp_box_colour);
	render_manager::rect(box.x + 1, box.y + 1, box.w - 2, box.h - 2, color(25, 25, 25, 120));
	render_manager::rect(box.x - 1, box.y - 1, box.w + 2, box.h + 2, color(25, 25, 25, 120));
}

void features::visuals::render_health_bar(esp_box box, int entity_index) {
	c_base_entity* entity = interfaces::client_ent_list->get_client_entity(entity_index);

	int pixel_value = entity->health() * box.h / 100;

	render_manager::filled_rect(box.x - 6, box.y - 1, 4, box.h + 2, color(0, 0, 0, 240));
	render_manager::filled_rect(box.x - 5, box.y + box.h - pixel_value, 2, pixel_value, color(0, 255, 0));
}

void features::visuals::render_health(esp_box box, int entity_index) {
	c_base_entity* entity = interfaces::client_ent_list->get_client_entity(entity_index);

	std::string final_text = std::to_string(entity->health()) + " hp";

	render_manager::text(final_text.c_str(), box.x + box.w + 2, box.y + y_position_displace, false, c_config::visuals::esp_health_colour);

	y_position_displace += 12;
}

void features::visuals::render_armour(esp_box box, int entity_index) {
	c_base_entity* entity = interfaces::client_ent_list->get_client_entity(entity_index);

	std::string final_text = std::to_string(entity->armour()) + " ap";

	render_manager::text(final_text.c_str(), box.x + box.w + 2, box.y + y_position_displace, false, c_config::visuals::esp_armour_colour);

	y_position_displace += 12;
}

void features::visuals::render_money(esp_box box, int entity_index) {
	c_base_entity* entity = interfaces::client_ent_list->get_client_entity(entity_index);

	std::string final_text = "$ " + std::to_string(entity->money());

	render_manager::text(final_text.c_str(), box.x + box.w + 2, box.y + y_position_displace, false, c_config::visuals::esp_money_colour);

	y_position_displace += 12;
}

void features::visuals::render_weapon(esp_box box, int entity_index) {
	c_base_entity* entity = interfaces::client_ent_list->get_client_entity(entity_index);

	auto current_weapon = entity->active_weapon();
	if (!current_weapon) return;

	std::string weapon_name = current_weapon->sanitized_weapon_name();

	std::transform(weapon_name.begin(), weapon_name.end(), weapon_name.begin(), ::tolower);

	ImVec2 thing = ImGui::CalcTextSize(weapon_name.c_str());

	int difference = thing.x / 2;

	render_manager::text(weapon_name.c_str(), (box.x + box.w / 2) - difference, box.y + box.h + 2, false, c_config::visuals::esp_weapon_colour);
}

void features::visuals::render_name(esp_box box, int entity_index) {
	static auto clean_name = [](char * name) -> std::string {
		name[127] = '\0';

		std::string tmp(name);

		if (tmp.length() > 20) {
			tmp.erase(20, (tmp.length() - 20));
			tmp.append("...");
		}

		return tmp;
	};

	player_info_t info;

	interfaces::engine_client->get_player_info(entity_index, &info);

	std::string name = clean_name(info.name);
	std::string bot, final_string;

	if (info.fakeplayer)
		bot = "[bot] ";

	final_string = bot + name;

	ImVec2 thing = ImGui::CalcTextSize(final_string.c_str());

	int difference = thing.x / 2;

	render_manager::text(final_string.c_str(), (box.x + box.w / 2) - difference, box.y - thing.y - 2, false, c_config::visuals::esp_name_colour);
}

void features::visuals::render_lines(c_base_entity* local)
{
	if (!c_config::visuals::angle_lines) return;

	auto drawAngleLine = [&](const vec3_t& origin, const vec3_t& w2sOrigin, const float& angle, const char* text, color clr) {
		vec3_t forward;
		global_math::angle_vectors(vec3_t(0.0f, angle, 0.0f), &forward);
		float AngleLinesLength = 30.0f;

		vec3_t w2sReal;
		if (global_utils::world_to_screen(origin + forward * AngleLinesLength, w2sReal)) {
			render_manager::draw_list->AddLine(ImVec2(w2sOrigin.x, w2sOrigin.y), ImVec2(w2sReal.x, w2sReal.y), ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1)));
			render_manager::text(text, w2sReal.x, w2sReal.y - 5.0f, true, clr);

		}
	};

	vec3_t w2sOrigin;
	if (global_utils::world_to_screen(local->abs_origin(), w2sOrigin)) {
		auto animstate = local->get_anim_state();
		drawAngleLine(local->abs_origin(), w2sOrigin, c_config::visuals::fake.y, "fake", color(0.0f, 255, 255, 255));
		drawAngleLine(local->abs_origin(), w2sOrigin, c_config::visuals::real.y, "real", color(255, 255, 0.0f, 255));
	}
}

#define fill(x, s) if(x.state) vec.push_back(s)

void features::visuals::render_binds()
{
	if (!c_config::visuals::show_binds) return;

	std::vector<std::string> vec;

	fill(c_config::legit::autofire, "Autofire");
	fill(c_config::triggerbot::key, "Triggerbot");
	fill(c_config::misc::legit_aa_inverter, "Desync Inverted");
	fill(c_config::rage::manual_back, "Back");
	fill(c_config::rage::manual_left, "Left");
	fill(c_config::rage::manual_right, "Right");

	ImGui::SetNextWindowSize(ImVec2(200.0f, 0.0f));
	ImGui::Begin("Key binds", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	for (auto i : vec)
		ImGui::Text(i.c_str());

	ImGui::End();
}

void features::visuals::render_fovs(c_base_entity* local) // shit seems to me i dont know math
{
	if (!c_config::visuals::show_fov) return;
	auto weapon = local->active_weapon();
	if (!weapon);

	auto data = weapon->get_cs_weapon_data();
	if (!data) return;

	if (data->iWeaponType == cs_weapon_type::WEAPONTYPE_C4 || data->iWeaponType == cs_weapon_type::WEAPONTYPE_GRENADE ||
		data->iWeaponType == cs_weapon_type::WEAPONTYPE_KNIFE) return;

	int shots = local->shots();
	int id = weapon->get_config_id();
	int w, h;
	I::engine_client->get_screen_size(w, h);
	if (!shots) render_manager::draw_list->AddCircle(ImVec2(w / 2, h / 2), c_config::legit::weapons[id].silent_fov * 9.555, ImGui::GetColorU32(ImVec4(0, 0, 1, 1)), 20);
	if(shots + 1 >= c_config::legit::weapons[id].rcs_start_bullet) render_manager::draw_list->AddCircle(ImVec2(w / 2, h / 2), c_config::legit::weapons[id].rcs_fov * 9.555, ImGui::GetColorU32(ImVec4(0, 1, 0, 1)), 20);
	else render_manager::draw_list->AddCircle(ImVec2(w / 2, h / 2), c_config::legit::weapons[id].fov * 9.555, ImGui::GetColorU32(ImVec4(0, 1, 0, 1)), 20);
}

features::visuals::esp_box box_info;

void features::visuals::render() {
	if (!interfaces::engine_client->in_game_connected())
		return;

	auto local = interfaces::client_ent_list->get_client_entity(interfaces::engine_client->get_local_player());
	if (!local) return;
	if (local->is_alive())
	{
		this->render_binds();
		this->render_lines(local);
		this->render_fovs(local);
	}

	for (int i = 0; i < 65; ++i) {
		c_base_entity* entity = interfaces::client_ent_list->get_client_entity(i);

		if (!entity) continue;
		if ((entity->team() == local->team() && !c_config::visuals::esp_teammates) || entity->dormant() || !entity->is_alive() || entity == local || entity->get_client_class()->class_id != ClassId_CCSPlayer)
			continue;

		if (!this->calculate_box_position(entity, box_info))
			continue;

		y_position_displace = 1;

		if (c_config::visuals::esp_boxes)
			this->render_box(box_info);

		if (c_config::visuals::esp_names)
			this->render_name(box_info, i);

		if (c_config::visuals::esp_weapon)
			this->render_weapon(box_info, i);

		if (c_config::visuals::esp_health)
			this->render_health(box_info, i);

		if (c_config::visuals::esp_health_bar)
			this->render_health_bar(box_info, i);

		if (c_config::visuals::esp_armour)
			this->render_armour(box_info, i);

		if (c_config::visuals::esp_money)
			this->render_money(box_info, i);
	}
}

bool found = false;
void* colorvar = nullptr;

void set_glow_color(color c, i_material* mat)
{
	colorvar = mat->find_var("$envmaptint", &found);
	if (found)
	{
		using fn = void(__thiscall*)(void*, float, float, float);
		(*(fn**)colorvar)[11](colorvar, (float)c.r / 255.f, (float)c.g / 255.f, (float)c.b / 255.f);
	}
}

bool features::visuals::render_chams(hooks::draw_model_execute_t o_dme, int edx, i_mat_render_context* ctx, const draw_model_state_t& state, const model_render_info_t& info, matrix3x4_t* matrix)
{
	i_material* mat = nullptr;

	static i_material* material = nullptr;
	static i_material* material_glow = nullptr;
	static i_material* material_metallic = nullptr;

	auto local = I::client_ent_list->get_client_entity(I::engine_client->get_local_player());

	if (local->dormant()) return true;

	if (!material)
		material = I::mat_system->find_material("debug/debugambientcube", nullptr);
	if (!material_metallic)
		material_metallic = I::mat_system->find_material("models/inventory_items/trophy_majors/crystal_clear", nullptr);
	if (!material_glow)
		material_glow = I::mat_system->find_material("dev/glow_armsrace.vmt", nullptr);

	auto mdl = info.pmodel;
	if (strstr(mdl->szName, "models/player"))
	{	
		c_base_entity* ent = I::client_ent_list->get_client_entity(info.entity_index);
		if (!ent) return true;
		if (!ent->is_alive() || ent->get_client_class()->class_id != ClassId_CCSPlayer) return true;
		bool is_enemy = ent->team() != local->team();
		if (!c_config::visuals::chams_teammates && !is_enemy && ent != local) return true;

		if (c_config::visuals::chams)
		{
			if (local->is_alive())
			{
				auto weapon = local->active_weapon();
				if (!weapon) goto ok;
				int id = weapon->get_config_id();
				if (is_enemy && c_config::visuals::chams_backtrack && c_config::legit::weapons[id].legit_backtrack)
				{
					material->set_flag(MATERIAL_VAR_IGNOREZ, true);
					material->color_modulate(c_config::visuals::chams_backtrack_color.r, c_config::visuals::chams_backtrack_color.g,
						c_config::visuals::chams_backtrack_color.b);
					I::model_render->override_material(material);

					for (int i = 0; i < 14; i++)
						o_dme(I::model_render, edx, ctx, state, info, lag_comp->backtrack[info.entity_index][i].bt_matrix);

					I::model_render->override_material(nullptr);
				}
			}

		ok:			

			switch (c_config::visuals::chams_type)
			{
			case 0: mat = material; break;
			case 1: mat = material_metallic; break;
			case 2: mat = material_glow; break;
			}

			mat->alpha_modulate(1.0f);

			if (c_config::visuals::chams_ignore_z)
			{
				if (is_enemy)
				{
					if (c_config::visuals::chams_type != 2) mat->color_modulate(c_config::visuals::chams_colour_invis_e.r,
						c_config::visuals::chams_colour_invis_e.g, c_config::visuals::chams_colour_invis_e.b);
					else set_glow_color(c_config::visuals::chams_colour_invis_e, mat);
				}
				else if (c_config::visuals::chams_teammates)
				{
					if (c_config::visuals::chams_type != 2) mat->color_modulate(c_config::visuals::chams_colour_invis_t.r, 
						c_config::visuals::chams_colour_invis_t.g, c_config::visuals::chams_colour_invis_t.b);
					else set_glow_color(c_config::visuals::chams_colour_invis_t, mat);
				}
					

				mat->set_flag(MATERIAL_VAR_IGNOREZ, true);

				I::model_render->override_material(mat);
				o_dme(I::model_render, edx, ctx, state, info, matrix);
			}

			if (is_enemy)
			{
				if (c_config::visuals::chams_type != 2) mat->color_modulate(c_config::visuals::chams_colour_vis_e.r,
					c_config::visuals::chams_colour_vis_e.g, c_config::visuals::chams_colour_vis_e.b);
				else set_glow_color(c_config::visuals::chams_colour_vis_e, mat);
			}
			else if (c_config::visuals::chams_teammates)
			{
				if (c_config::visuals::chams_type != 2) mat->color_modulate(c_config::visuals::chams_colour_vis_t.r, 
					c_config::visuals::chams_colour_vis_t.g, c_config::visuals::chams_colour_vis_t.b);
				else set_glow_color(c_config::visuals::chams_colour_vis_t, mat);
			}

			mat->set_flag(MATERIAL_VAR_IGNOREZ, false);
			I::model_render->override_material(mat);
		}
	}
	else if (strstr(mdl->szName, "sleeves"))
	{
		if (c_config::visuals::chams_arms) return false;
	}
	else if (strstr(mdl->szName, "arms"))
	{
		if (c_config::visuals::chams_arms)
		{
			switch (c_config::visuals::chams_arms_type)
			{
			case 0: mat = material; break;
			case 1: mat = material_metallic; break;
			case 2: mat = material_glow; break;
			}

			mat->set_flag(MATERIAL_VAR_IGNOREZ, false);
			mat->set_flag(MATERIAL_VAR_WIREFRAME, c_config::visuals::chams_arms_wireframe);

			if (c_config::visuals::chams_arms_type != 2) mat->color_modulate(c_config::visuals::chams_arms_color.r, c_config::visuals::chams_arms_color.g, c_config::visuals::chams_arms_color.b);
			else set_glow_color(c_config::visuals::chams_arms_color, mat);

			mat->alpha_modulate(c_config::visuals::chams_arms_alpha);

			I::model_render->override_material(mat);
		}
	}
	else if (strstr(mdl->szName, "weapons/v_"))
	{
		if (c_config::visuals::chams_weapons)
		{
			switch (c_config::visuals::chams_weapons_type)
			{
			case 0: mat = material; break;
			case 1: mat = material_metallic; break;
			case 2: mat = material_glow; break;
			}

			mat->set_flag(MATERIAL_VAR_IGNOREZ, false);
			mat->set_flag(MATERIAL_VAR_WIREFRAME, c_config::visuals::chams_weapons_wireframe);

			if (c_config::visuals::chams_weapons_type != 2) mat->color_modulate(c_config::visuals::chams_weapons_color.r, c_config::visuals::chams_weapons_color.g, c_config::visuals::chams_weapons_color.b);
			else set_glow_color(c_config::visuals::chams_weapons_color, mat);

			mat->alpha_modulate(c_config::visuals::chams_weapons_alpha);

			I::model_render->override_material(mat);
		}
	}

	return true;
}

extern vec3_t vangles;
void features::visuals::thirdperson()
{
	c_base_entity* local = I::client_ent_list->get_client_entity(I::engine_client->get_local_player());
	if (!local || !I::engine_client->in_game_connected()) return;

	if (!local->is_alive()) // yes
	{
		local->m_iObserverMode() = c_config::visuals::thirdperson_key.state ? 5 : 4;
		I::input->m_f_cam_in_thirdperson = false;
		return;
	}
	else local->m_iObserverMode() = 0;

	I::input->m_f_cam_in_thirdperson = c_config::visuals::thirdperson_key.state;

	if (I::input->m_f_cam_in_thirdperson)
	{
		float dist = c_config::visuals::thirdperson_dist;

		vec3_t* view = local->get_vangles();

		vec3_t desiredCamOffset = vec3_t(std::cos(global_math::deg2rad(view->x)) * dist,
			std::sin(global_math::deg2rad(view->y)) * dist,
			std::sin(global_math::deg2rad(-view->x)) * dist
		);

		c_trace tr;
		c_ray ray(local->origin() + local->view_offset(), (local->origin() + local->view_offset() - desiredCamOffset));

		c_trace_filter traceFilter(local);
		I::engine_trace->trace_ray(ray, mask_shot, &traceFilter, &tr);

		vec3_t diff = local->origin() + local->view_offset() - tr.end;

		float distance2D = std::sqrt(std::abs(diff.x * diff.x) + abs(diff.y * diff.y));

		bool horOK = distance2D > (dist - 2.0f);
		bool vertOK = (abs(diff.z) - abs(desiredCamOffset.z) < 3.0f);

		float cameraDistance;

		if (horOK && vertOK)  
		{
			cameraDistance = dist; 
		}
		else
		{
			if (vertOK) 
			{
				cameraDistance = distance2D * 0.95f;
			}
			else
			{
				cameraDistance = abs(diff.z) * 0.95f;
			}
		}

		I::input->m_vec_camera_offset.z = cameraDistance;

		I::game_prediction->set_local_view_angles(vangles);
	}
}

void features::visuals::modify_smoke()
{
	static bool last = c_config::visuals::wireframe_smoke;

	if (!I::engine_client->in_game_connected())
	{
		last = !c_config::visuals::wireframe_smoke;
		return;
	}

	if (last != c_config::visuals::wireframe_smoke)
	{
		static const char* materials[] = {
			"particle/vistasmokev1/vistasmokev1_emods",
			"particle/vistasmokev1/vistasmokev1_emods_impactdust",
			"particle/vistasmokev1/vistasmokev1_fire",
			"particle/vistasmokev1/vistasmokev1_smokegrenade"
		};

		for (auto mat : materials)
		{
			auto material = I::mat_system->find_material(mat, TEXTURE_GROUP_PARTICLE);
			material->set_flag(MATERIAL_VAR_WIREFRAME, c_config::visuals::wireframe_smoke);
		}

		last = c_config::visuals::wireframe_smoke;
	}
}

void features::visuals::nightmode()
{
	static bool last = c_config::visuals::nightmode;

	if (!I::engine_client->in_game_connected())
	{
		last = !c_config::visuals::nightmode;
		return;
	}

	if (last != c_config::visuals::nightmode)
	{
		last = c_config::visuals::nightmode;

		for (auto i = I::mat_system->first_material(); i != I::mat_system->invalid_material_handle(); i = I::mat_system->next_material(i))
		{
			auto mat = I::mat_system->get_material(i);
			if (!mat) continue;
			if (strstr(mat->get_group_name(), "World textures"))			if(last) mat->color_modulate(0.10f * 255.0f, 0.10f * 255.0f, 0.10f * 255.0f); else mat->color_modulate(255.0f, 255.0f, 255.0f);
			if (strstr(mat->get_group_name(), "StaticProp"))				if(last) mat->color_modulate(0.30f * 255.0f, 0.30f * 255.0f, 0.30f * 255.0f); else mat->color_modulate(255.0f, 255.0f, 255.0f);
			if (strstr(mat->get_group_name(), "Particle textures"))			mat->set_flag(MATERIAL_VAR_NO_DRAW, last ? true : false);
		}

		auto cvar = I::game_cvar->find_var("sv_skyname");
		cvar->set_value("sky_csgo_night02");
	}
}