#include "ragebot.h"
#include "autowall.h"
#include "../legit/legit_lag_comp.h"
#include "../engine_prediction/engine_prediction.h"

#include "../../sdk/sdk.h"
#include "../../interfaces/interfaces.h"
#include "../../utilities/globals.h"
#include "../../utilities/math.h"
#include "../../menu/settings.h"

struct aimpoint
{
	aimpoint(vec3_t point, vec3_t center, int hitbox)
	{
		this->point = point;
		this->center = center;
		this->hitbox = hitbox;
	}

	vec3_t point;
	vec3_t center;
	int hitbox;
};

bool features::ragebot::hitchance(vec3_t angles, float chance, c_base_entity* ent)
{
	auto weapon = G::local_player->active_weapon();

	if (!weapon)
		return false;

	vec3_t forward, right, up;
	vec3_t src = engine_pred->unpred_eyepos;
	global_math::angle_vectors(angles, &forward, &right, &up);

	int cHits = 0;
	int cNeededHits = static_cast<int>(256.f * (chance / 100.f));

	weapon->update_accuracy_penalty();
	float weap_spread = weapon->get_spread();
	float weap_inaccuracy = weapon->get_inaccuracy();

	for (int i = 0; i < 256; i++)
	{
		float a = global_utils::random_float(0.f, 1.f);
		float b = global_utils::random_float(0.f, 2.f * 3.14159265358979323846);
		float c = global_utils::random_float(0.f, 1.f);
		float d = global_utils::random_float(0.f, 2.f * 3.14159265358979323846);

		float inaccuracy = a * weap_inaccuracy;
		float spread = c * weap_spread;

		if (weapon->get_item_definition_index() == item_definition_index::WEAPON_REVOLVER)
		{
			a = 1.f - a * a;
			a = 1.f - c * c;
		}

		vec3_t spreadView((cos(b) * inaccuracy) + (cos(d) * spread), (sin(b) * inaccuracy) + (sin(d) * spread), 0), direction;

		direction.x = forward.x + (spreadView.x * right.x) + (spreadView.y * up.x);
		direction.y = forward.y + (spreadView.x * right.y) + (spreadView.y * up.y);
		direction.z = forward.z + (spreadView.x * right.z) + (spreadView.y * up.z);
		direction.Normalized();

		vec3_t viewAnglesSpread;
		global_math::vector_angles2(direction, up, viewAnglesSpread);
		global_math::normalize_angles(viewAnglesSpread);

		vec3_t viewForward;
		global_math::angle_vectors(viewAnglesSpread, &viewForward);
		viewForward.NormalizeInPlace();

		viewForward = src + (viewForward * weapon->get_cs_weapon_data()->flRange);

		c_trace tr;
		c_ray ray(src, viewForward);

		I::engine_trace->clip_ray_to_entity(ray, mask_shot | contents_grate, ent, &tr);

		if (tr.entity == ent)
			++cHits;

		if (static_cast<int>((static_cast<float>(cHits) / 256.f) * 100.f) >= chance)
			return true;

		if ((256 - i + cHits) < cNeededHits)
			return false;
	}
	return false;
}

void multipoints(features::backtrack_tick record, std::vector<aimpoint>& points, c_base_entity* entity, c_config::rage::_weapons cfg)
{
	auto studio_hdr = I::model_info->get_studio_model(entity->GetModel());
	if (!studio_hdr) return;
	
	for (int i = 0; i < hitbox_max - 1; i++)
	{
		if (!cfg.hitboxes[0] && i == hitboxes::hitbox_head) continue;
		if (!cfg.hitboxes[1] && (i == hitboxes::hitbox_lower_neck || i == hitboxes::hitbox_neck)) continue;
		if (!cfg.hitboxes[2] && (i == hitboxes::hitbox_lower_chest || i == hitboxes::hitbox_chest || i == hitboxes::hitbox_upper_chest ||
			i == hitboxes::hitbox_stomach || i == hitboxes::hitbox_pelvis)) continue;
		if (!cfg.hitboxes[3] && (i == hitboxes::hitbox_right_hand || i == hitboxes::hitbox_left_hand || i == hitboxes::hitbox_right_upper_arm ||
			i == hitboxes::hitbox_right_forearm || i == hitboxes::hitbox_left_upper_arm || i == hitboxes::hitbox_left_forearm)) continue;
		if (!cfg.hitboxes[4] && (i == hitboxes::hitbox_right_thigh || i == hitboxes::hitbox_left_thigh || i == hitbox_right_calf ||
			i == hitboxes::hitbox_left_calf || i == hitboxes::hitbox_right_foot || i == hitboxes::hitbox_left_foot)) continue;

		auto hitbox = studio_hdr->hitbox(i, 0);
		if (!hitbox) continue;
		
		auto& mat = record.bt_matrix[hitbox->bone];
		const auto mod = hitbox->radius != -1.f ? hitbox->radius : 0.f;

		vec3_t min = global_math::vector_transform(hitbox->mins + mod, mat);
		vec3_t max = global_math::vector_transform(hitbox->maxs + mod, mat);
		const auto center = (min + max) * 0.5f;
		points.emplace_back(center, center, i);	
		auto angles = global_math::calculate_angle(center, features::engine_pred->unpred_eyepos);
		vec3_t forward;
		global_math::angle_vectors(angles, &forward);

		auto rs = hitbox->radius * (float)cfg.body_scale / 100.f;
		if (i == hitbox_head)
		{
			rs = hitbox->radius * (float)cfg.head_scale / 100.f;
		}

		if (rs < 0.2)
			continue;

		const auto right = forward.Cross(vec3_t(0, 0, 1)) * rs;
		const auto left = vec3_t(-right.x, -right.y, right.z);
		const auto top = vec3_t(0, 0, 1) * rs;

		if (i == hitbox_head)
			points.emplace_back(center + top, center + top * 0.95f, i);
		points.emplace_back(center + right, center + top * 0.95f, i);
		points.emplace_back(center + left, center + top * 0.95f, i);
	}
}

aimpoint get_best_damage(features::backtrack_tick record, float& damage, c_base_entity* entity, c_config::rage::_weapons cfg)
{
	std::vector<aimpoint> points;
	multipoints(record, points, entity, cfg);

	auto was_repeated = false;

	auto best_damage = 0.f;
	aimpoint best_point = { vec3_t(0.f, 0.f, 0.f), vec3_t(0.f, 0.f, 0.f), -1 };

	auto prevhitbox = -1;

	for (auto& point : points)
	{
		float point_damage = 0.0f;

		if (!autowall::can_hit(entity, point.point, point_damage, features::engine_pred->unpred_eyepos, cfg))
			continue;

		if ((point_damage > best_damage && prevhitbox != point.hitbox || point_damage > best_damage + 30.f))
		{
			best_point = point;
			best_damage = point_damage;
			prevhitbox = point.hitbox;
		}
	}

	damage = best_damage;
	return best_point;
}

c_base_entity* find_best_ent(c_config::rage::_weapons cfg, c_base_combat_weapon* weapon, aimpoint& pt, int& best_record)
{
	c_base_entity* best_ent = nullptr;
	aimpoint best_point(vec3_t(0, 0, 0), vec3_t(0, 0, 0), -1);
	float best_damage = 0.0;

	for (int i = 0; i <= 1; i++)
	{
		auto ent = I::client_ent_list->get_client_entity(i);
		if (!ent || ent == G::local_player) continue;
		if (!ent->is_alive() || ent->dormant() || ent->gun_game_immunity() || ent->team() == G::local_player->team()) continue;

		auto mdl = I::model_info->get_studio_model(ent->model());
		if (!mdl) continue;
		
		for (int j = 0; j < 1; j++)
		{
			auto rec = features::lag_comp->backtrack[i][j];
			float dmg = 0.0f;
			auto point = get_best_damage(rec, dmg, ent, cfg);

			if (dmg > best_damage)
			{
				best_damage = dmg;
				best_point = point;
				best_ent = ent;
				best_record = j;
			}
		}
	}

	pt = best_point;
	return best_ent;
}

extern void quick_stop();
extern bool can_shoot(const float);

void features::ragebot::on_create_move()
{
	if (!I::engine_client->in_game_connected() || !G::local_player || !c_config::rage::enabled) return;
	if (!G::local_player->is_alive() || G::local_player->dormant() || !can_shoot(I::global_vars->curtime)) return;

	auto weapon = G::local_player->active_weapon();
	if (!weapon) return;
	auto data = weapon->get_cs_weapon_data();
	if (weapon->get_next_primary_attack() > I::global_vars->curtime) return;
	
	int id = weapon->get_rage_config_id();
	if (id == -1) return;

	aimpoint pt(vec3_t(0, 0, 0), vec3_t(0, 0, 0), -1);
	int best_record = -1;
	c_base_entity* ent = find_best_ent(c_config::rage::weapons[id], weapon, pt, best_record);
	if (!ent) return;

	if (c_config::rage::autostop)
		quick_stop();

	if (c_config::rage::autoscope && !G::local_player->scoped() && data->iWeaponType == cs_weapon_type::WEAPONTYPE_SNIPER_RIFLE
		&& !(G::global_cmd->buttons & in_attack2) && !(G::global_cmd->buttons & in_attack))
	{
		G::global_cmd->buttons |= in_attack2;
		return;
	}

	auto angles = global_math::calculate_angle(engine_pred->unpred_eyepos, pt.point);
	angles.clamp();
	if (!this->hitchance(angles, c_config::rage::weapons[id].hitchance, ent))
		return;

	G::global_cmd->viewangles = angles;
	if (c_config::rage::autofire && !(G::global_cmd->buttons & in_attack))
		G::global_cmd->buttons |= in_attack;

	if (G::global_cmd->buttons & in_attack)
	{
		G::ShotsFired[ent->ent_index()]++;
		G::global_cmd->tick_count = (int)(0.5f + (float)(lag_comp->backtrack[ent->ent_index()][best_record].simulation_time) / interfaces::global_vars->interval_per_tick);
		if (weapon->get_item_definition_index() != item_definition_index::WEAPON_AWP && weapon->get_item_definition_index() != item_definition_index::WEAPON_SSG08)
			G::global_cmd->viewangles -= G::local_player->aim_punch_angle() * 2.f;
	}

	G::global_cmd->viewangles.clamp();
}

void features::ragebot::apply_aa()
{
	if (!I::engine_client->in_game_connected() || !G::local_player || !c_config::rage::antiaims_enabled) return;
	if (!G::local_player->is_alive()) return;
	if (G::global_cmd->buttons & in_attack || G::global_cmd->buttons & in_use) return;
	if (global_variables::local_player->move_type() == move_types::ladder) return;

	c_base_combat_weapon* weapon = G::local_player->active_weapon();
	if (!weapon)
		return;

	item_definition_index weapon_index = weapon->get_item_definition_index();
	if ((weapon_index == item_definition_index::WEAPON_GLOCK || weapon_index == item_definition_index::WEAPON_FAMAS) && weapon->get_next_primary_attack() >= I::global_vars->curtime)
		return;

	weapon_info_t* weapon_data = weapon->get_cs_weapon_data();

	if (weapon_data->iWeaponType == cs_weapon_type::WEAPONTYPE_GRENADE) {
		if (!weapon->m_bPinPulled()) {
			if (weapon->m_fThrowTime() > 0.f)
				return;
		}

		if ((G::global_cmd->buttons & in_attack) || (G::global_cmd->buttons & in_attack2)) {
			if (weapon->m_fThrowTime() > 0.f)
				return;
		}
	}

	bool is_moving = G::local_player->velocity().Length2D() > 5.0f;
	bool in_air = !(G::local_player->flags() & fl_onground);

	if (in_air)
	{
		switch (c_config::rage::antiaims[2].pitch)
		{
		case 1: G::global_cmd->viewangles.x = 89.0f; break;
		case 2: G::global_cmd->viewangles.x = -89.0f; break;
		}

		if (c_config::rage::antiaims[2].yaw == 1)
			G::global_cmd->viewangles.y += 180.0f;
		else if (c_config::rage::antiaims[2].yaw == 2)
		{
			if (c_config::rage::manual_back.state)
				G::global_cmd->viewangles.y += 180.0f;
			if (c_config::rage::manual_left.state)
				G::global_cmd->viewangles.y += 90.0f;
			if (c_config::rage::manual_right.state)
				G::global_cmd->viewangles.y -= 90.0f;

			if (!c_config::rage::manual_back.state && !c_config::rage::manual_left.state && !c_config::rage::manual_right.state)
				G::global_cmd->viewangles.y += 180.0f;
		}
		else if (c_config::rage::antiaims[2].yaw == 3)
			G::global_cmd->viewangles.y = global_utils::random_float(-180.0f, 180.0f);
	}
	else if (is_moving)
	{
		switch (c_config::rage::antiaims[1].pitch)
		{
		case 1: G::global_cmd->viewangles.x = 89.0f; break;
		case 2: G::global_cmd->viewangles.x = -89.0f; break;
		}

		if (c_config::rage::antiaims[1].yaw == 1)
			G::global_cmd->viewangles.y += 180.0f;
		else if (c_config::rage::antiaims[1].yaw == 2)
		{
			if (c_config::rage::manual_back.state)
				G::global_cmd->viewangles.y += 180.0f;
			if (c_config::rage::manual_left.state)
				G::global_cmd->viewangles.y += 90.0f;
			if (c_config::rage::manual_right.state)
				G::global_cmd->viewangles.y -= 90.0f;

			if (!c_config::rage::manual_back.state && !c_config::rage::manual_left.state && !c_config::rage::manual_right.state)
				G::global_cmd->viewangles.y += 180.0f;
		}
		else if (c_config::rage::antiaims[1].yaw == 3)
			G::global_cmd->viewangles.y = global_utils::random_float(-180.0f, 180.0f);
	}
	else
	{
		switch (c_config::rage::antiaims[0].pitch)
		{
		case 1: G::global_cmd->viewangles.x = 89.0f; break;
		case 2: G::global_cmd->viewangles.x = -89.0f; break;
		}

		if (c_config::rage::antiaims[0].yaw == 1)
			G::global_cmd->viewangles.y += 180.0f;
		else if (c_config::rage::antiaims[0].yaw == 2)
		{
			if (c_config::rage::manual_back.state)
				G::global_cmd->viewangles.y += 180.0f;
			if (c_config::rage::manual_left.state)
				G::global_cmd->viewangles.y += 90.0f;
			if (c_config::rage::manual_right.state)
				G::global_cmd->viewangles.y -= 90.0f;

			if (!c_config::rage::manual_back.state && !c_config::rage::manual_left.state && !c_config::rage::manual_right.state)
				G::global_cmd->viewangles.y += 180.0f;
		}
		else if (c_config::rage::antiaims[0].yaw == 3)
			G::global_cmd->viewangles.y = global_utils::random_float(-180.0f, 180.0f);
	}

	G::global_cmd->viewangles.clamp();
}