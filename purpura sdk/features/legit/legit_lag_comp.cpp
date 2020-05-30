#include "legit_lag_comp.h"

#include "../../utilities/globals.h"
#include "../../menu/settings.h"

matrix3x4_t bone_matrix[128];

void features::legit_lag_comp::on_create_move() {
	int best_target = -1;
	float best_fov = 90.f;

	if (!interfaces::engine_client->in_game_connected() || !global_variables::local_player || !c_config::legit::legit_aimbot && !c_config::rage::enabled) 
		return;

	if (global_variables::local_player->health() <= 0)
		return;

	c_base_combat_weapon* weapon = global_variables::local_player->active_weapon();
	if (!weapon) 
		return;

	int index = weapon->get_config_id();
	if (!c_config::legit::weapons[index].legit_backtrack && !c_config::rage::enabled || !index)
		return;

	for (int i = 0; i <= I::global_vars->max_clients; i++) {
		c_base_entity* ent = interfaces::client_ent_list->get_client_entity(i);

		if (!ent) continue;
		if (ent->health() < 0) continue;
		if (ent->dormant()) continue;
		if (ent->team() == global_variables::local_player->team()) continue;

		if (ent->health() > 0) {
			float sim_time = ent->simulation_time();
			ent->setup_bones(bone_matrix, 128, 0x0100, /*0.f*/ I::global_vars->curtime);

			backtrack[i][global_variables::global_cmd->command_number % 14] = backtrack_tick
			{
				sim_time,
				ent->bone_pos(8),
				ent->abs_origin(),
				ent->abs_angles(),
				bone_matrix[128]
			};

			memcpy(backtrack[i][global_variables::global_cmd->command_number % 14].bt_matrix, bone_matrix, sizeof(matrix3x4_t) * 128);

			vec3_t view_direction = angle_vector(global_variables::global_cmd->viewangles);
			float fov = distance_point_to_line(ent->bone_pos(8), global_variables::local_player->bone_pos(8), view_direction);

			if (best_fov > fov) {
				best_fov = fov;
				best_target = i;
			}
		}
	}
	float best_target_simulation_time = 0.f;

	if (best_target != -1) {
		float temp = FLT_MAX;
		vec3_t view_direction = angle_vector(global_variables::global_cmd->viewangles);

		for (int t = 0; t < 14; ++t) {
			float fov = distance_point_to_line(backtrack[best_target][t].hitbox_position, global_variables::local_player->bone_pos(8), view_direction);
			if (temp > fov && backtrack[best_target][t].simulation_time > global_variables::local_player->simulation_time() - 1) {
				temp = fov;
				best_target_simulation_time = backtrack[best_target][t].simulation_time;
			}
		}

		if (global_variables::global_cmd->buttons & in_attack || !c_config::rage::enabled) {
			global_variables::global_cmd->tick_count = (int)(0.5f + (float)(best_target_simulation_time) / interfaces::global_vars->interval_per_tick);
		}
	}
}