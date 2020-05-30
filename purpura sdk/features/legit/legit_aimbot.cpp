#include "legit_aimbot.h"
#include "../rage/ragebot.h"

#include "../../utilities/globals.h"
#include "../../utilities/math.h"
#include "../../interfaces/interfaces.h"
#include "../../menu/settings.h"

vec3_t old_punch_angle = { 0, 0, 0 };

bool does_line_goes_through_smoke(const vec3_t& vStartPos, const vec3_t& vEndPos) {
	using LineGoesThroughSmokeFn = bool(*) (vec3_t, vec3_t, int16_t);
	static auto fnLineGoesThroughSmoke = global_utils::get_signature("client_panorama.dll", "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0");
	return reinterpret_cast<LineGoesThroughSmokeFn>(fnLineGoesThroughSmoke)(vStartPos, vEndPos, 1);
}

bool can_shoot(const float time)
{
	auto weapon = G::local_player->active_weapon();
	if (!weapon)
		return false;

	const auto weapontype = weapon->get_cs_weapon_data()->iWeaponType;
	if (weapontype == cs_weapon_type::WEAPONTYPE_C4 || weapontype == cs_weapon_type::WEAPONTYPE_GRENADE)
		return false;

	if (weapon->get_ammo() < 1)
		return false;

	if (weapon->get_next_primary_attack() > time)
		return false;

	return true;
}

void quick_stop()
{
	G::global_cmd->viewangles.clamp();

	auto vel2 = G::local_player->velocity();
	const auto speed = vel2.Length();
	if (speed > 15.f)
	{
		vec3_t dir;
		global_math::vector_angles(vel2, dir);
		dir.y = G::global_cmd->viewangles.y - dir.y;

		vec3_t new_move;
		global_math::angle_vectors(dir, &new_move);
		const auto max = max(std::fabs(G::global_cmd->forwardmove), std::fabs(G::global_cmd->sidemove));
		const auto mult = 450.f / max;
		new_move *= -mult;

		G::global_cmd->forwardmove = new_move.x;
		G::global_cmd->sidemove = new_move.y;
	}
	else
	{
		G::global_cmd->forwardmove = 0.f;
		G::global_cmd->sidemove = 0.f;
	}
}

void features::legit_aimbot::on_create_move()
{
	if (!G::local_player || !I::engine_client->in_game_connected() || !c_config::legit::legit_aimbot) return;
	if (!G::local_player->is_alive() || !(G::global_cmd->buttons & in_attack) && !c_config::legit::autofire.state || !can_shoot(I::global_vars->curtime)) return;
	if (G::local_player->flash_duration() > 1.0f && c_config::legit::disable_if_flashed) return;
	
	c_base_combat_weapon* weapon = G::local_player->active_weapon();
	if (!weapon) return;
	if (weapon->get_next_primary_attack() > I::global_vars->curtime) return;

	int id = weapon->get_config_id();
	if (!id) return;
	if (I::global_vars->curtime < stop_time && c_config::legit::weapons[id].kill_delay > 0) return;

	if ((id == 29 || id == 32 || id == 33 || id == 34) && !G::local_player->scoped() && c_config::legit::weapons[id].only_scoped) return;

	float best_fov = 0.0f;
	float smooth = 0.0f;
	vec3_t best_angle;
	vec3_t punch_angle = { 0.0f, 0.0f, 0.0f };
	c_base_entity* best_entity = nullptr;

	int shots = G::local_player->shots();
	if (shots >= c_config::legit::weapons[id].rcs_start_bullet && id > 14 && id != 32 && id != 29)
	{
		best_fov = c_config::legit::weapons[id].rcs_fov;
		smooth = c_config::legit::weapons[id].rcs_smooth;
		punch_angle = G::local_player->aim_punch_angle() * 2.f;
		punch_angle.x *= c_config::legit::weapons[id].rcs_y;
		punch_angle.y *= c_config::legit::weapons[id].rcs_x;
	}
	else
	{
		best_fov = max(c_config::legit::weapons[id].fov, c_config::legit::weapons[id].silent_fov);
		smooth = c_config::legit::weapons[id].smooth;
	}

	auto get_fov = [](vec3_t aim_angles) -> float {
		vec3_t delta = aim_angles - G::global_cmd->viewangles;
		delta.clamp();
		return delta.Length2D();
	};

	static int bones[] = { 0, 8, 7, 6, 5, 4, 3 };
	int c_bone = bones[c_config::legit::weapons[id].hitbox];
	vec3_t me = G::local_player->origin() + G::local_player->view_offset();

	for (int i = 0; i <= I::global_vars->max_clients; i++)
	{
		c_base_entity* ent = I::client_ent_list->get_client_entity(i);

		if (!ent || ent == G::local_player) continue;
		if (!ent->is_alive() || ent->dormant() || ent->get_client_class()->class_id != ClassId_CCSPlayer || ent->gun_game_immunity() ||
			(ent->team() == G::local_player->team() && !c_config::legit::attack_teammates) 
			|| (!ent->is_visible() && c_config::legit::visible_check)) continue;
		if (does_line_goes_through_smoke(me, ent->origin() + ent->view_offset()) && c_config::legit::disable_in_smoke) continue;
		
		if (!c_bone)
		{
			for (int j = 1; j < 7; j++)
			{
				vec3_t pos = ent->bone_pos(bones[j]);
				vec3_t angle = global_math::calculate_angle(me, pos);
				angle.clamp();

				float fov = get_fov(angle);
				if (fov <= best_fov)
				{
					best_fov = fov;
					best_entity = ent;
					best_angle = angle;
				}
			}
		}
		else
		{
			vec3_t pos = ent->bone_pos(bones[c_config::legit::weapons[id].hitbox]);
			vec3_t angle = global_math::calculate_angle(me, pos);
			angle.clamp();

			float fov = get_fov(angle);
			if (fov <= best_fov)
			{
				best_fov = fov;
				best_entity = ent;
				best_angle = angle;
			}
		}
	}

	if (!best_entity)
	{
		if (c_config::legit::weapons[id].rcs_standalone && G::global_cmd->buttons & in_attack && shots >= c_config::legit::weapons[id].rcs_start_bullet)
		{
			G::global_cmd->viewangles -= punch_angle;
			G::global_cmd->viewangles += old_punch_angle;
			G::global_cmd->viewangles.clamp();
			I::engine_client->set_view_angles(G::global_cmd->viewangles);
		}

		return;
	}

	if (c_config::legit::autostop && (G::global_cmd->buttons & in_attack) || c_config::legit::autofire.state)
		quick_stop();

	if (c_config::legit::autofire.state && !(G::global_cmd->buttons & in_attack) && !rage->hitchance(best_angle, c_config::legit::weapons[id].hitchance, best_entity))
		return;

	if (shots < 2 && best_fov <= c_config::legit::weapons[id].silent_fov)
	{
		G::global_cmd->viewangles = best_angle;
		if (c_config::legit::autofire.state && id != 29 && id != 32 && id > 14)
			G::global_cmd->viewangles -= punch_angle;
	}
	else
	{
		vec3_t delta = best_angle - punch_angle - G::global_cmd->viewangles;
		delta /= smooth;
		G::global_cmd->viewangles += delta;
		G::global_cmd->viewangles.clamp();
		I::engine_client->set_view_angles(G::global_cmd->viewangles);
	}

	if (c_config::legit::autofire.state && !(G::global_cmd->buttons & in_attack))
		G::global_cmd->buttons |= in_attack;
}

void features::legit_aimbot::autopistol()
{
	if (!I::engine_client->in_game_connected() || !G::local_player) 
		return;
	if (!G::local_player->is_alive()) 
		return;

	auto weapon = G::local_player->active_weapon();
	if (!weapon) 
		return;
	int id = weapon->get_config_id();
	if (!id || id > 9) 
		return;
	if (!c_config::legit::weapons[id].autopistol)
		return;

	if (weapon->get_next_primary_attack() > I::global_vars->interval_per_tick * G::local_player->tickbase())
		G::global_cmd->buttons &= ~in_attack;
}