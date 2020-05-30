#include "triggerbot.h"
#include "../rage/ragebot.h"

#include "../../utilities/globals.h"
#include "../../utilities/math.h"
#include "../../interfaces/interfaces.h"
#include "../../menu/settings.h"

extern bool does_line_goes_through_smoke(const vec3_t& vStartPos, const vec3_t& vEndPos);

void features::legit_triggerbot::on_create_move()
{
	if (G::global_cmd->buttons & in_attack || !G::local_player || !I::engine_client->in_game_connected()) return;
	if (!G::local_player->is_alive()) return;
	if (G::local_player->flash_duration() > 1.0f && c_config::triggerbot::disable_if_flashed) return;
	if (!c_config::triggerbot::key.state) return;

	c_base_combat_weapon* weapon = G::local_player->active_weapon();
	if (!weapon) return;
	if (weapon->get_next_primary_attack() > I::global_vars->curtime) return;

	auto data = weapon->get_cs_weapon_data();
	if (!data) return;

	int id = weapon->get_config_id();
	if (!id || !c_config::triggerbot::weapons[id].enabled) return;

	if ((id == 29 || id == 32 || id == 33 || id == 34) && !G::local_player->scoped() && c_config::triggerbot::weapons[id].only_scoped) return;

	vec3_t view_direction;
	global_math::angle_vectors(G::global_cmd->viewangles, &view_direction);
	view_direction *= data->flRange;

	vec3_t mypos = G::local_player->origin() + G::local_player->view_offset();
	vec3_t end_pos = mypos + view_direction;

	if (does_line_goes_through_smoke(mypos, end_pos) && c_config::triggerbot::disable_in_smoke) return;

	c_ray ray(mypos, end_pos);
	c_trace_filter filter(G::local_player);
	c_trace trace;
	I::engine_trace->trace_ray(ray, mask_shot, &filter, &trace);

	c_base_entity* ent = trace.entity;

	if (ent)
	{
		if (!ent->is_alive() || ent->dormant() || ent->gun_game_immunity() || ent->get_client_class()->class_id != ClassId_CCSPlayer) return;
		if (ent->team() == G::local_player->team()) return;

		switch (trace.hitGroup)
		{
		case hitgroups::hitgroup_head: if (!c_config::triggerbot::weapons[id].hitboxes[0]) return; break;
		case hitgroups::hitgroup_chest: if (!c_config::triggerbot::weapons[id].hitboxes[1]) return; break;
		case hitgroups::hitgroup_stomach: if (!c_config::triggerbot::weapons[id].hitboxes[2]) return; break;
		case hitgroups::hitgroup_leftarm: if (!c_config::triggerbot::weapons[id].hitboxes[3]) return; break;
		case hitgroups::hitgroup_rightarm: if (!c_config::triggerbot::weapons[id].hitboxes[4]) return; break;
		case hitgroups::hitgroup_leftleg: if (!c_config::triggerbot::weapons[id].hitboxes[5]) return; break;
		case hitgroups::hitgroup_rightleg: if (!c_config::triggerbot::weapons[id].hitboxes[6]) return; break;
		}

		if (rage->hitchance(G::global_cmd->viewangles, c_config::triggerbot::weapons[id].hitchance, ent))
			G::global_cmd->buttons |= in_attack;
	}
}