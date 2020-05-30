#include "legit_antiaim.h"

#include "../../utilities/globals.h"
#include "../../utilities/math.h"
#include "../../interfaces/interfaces.h"
#include "../../menu/settings.h"

void features::legit_antiaim::on_create_move()
{
	if (!c_config::misc::legit_aa && !c_config::rage::desync || !I::engine_client->in_game_connected() || !G::local_player) return;
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

	if (!reinterpret_cast<i_net_channel_info*>(I::engine_client->get_net_channel_info())->chokedPackets && !c_config::rage::fakelag)
		G::b_send_packet = false;

	static float mod = 1.0f;
	static bool up = false;

	if (c_config::rage::desync && G::global_cmd->command_number % 4 == 0)
	{
		if (mod <= 1.0f && mod > 0.0f && !up)
			mod -= 0.1f;
		else if (mod >= 0.0f && mod < 1.0f)
			mod += 0.1f;
	}

	mod = std::clamp(mod, 0.0f, 1.0f);
	if (!up) up = mod == 0.0f;
	if (mod == 1.0f) up = false;

	if (!c_config::rage::desync)
	{
		up = false;
		mod = 1.0f;
	}

	if (!G::b_send_packet)
	{		
		G::global_cmd->viewangles.y += G::local_player->max_desync() * (c_config::misc::legit_aa_inverter.state ? 1.0f : -1.0f) * mod;
		G::global_cmd->viewangles.clamp();	
	}

	if (G::local_player->velocity().Length2D() < 5.0f)
	{
		static bool sw = false;

		if (G::global_cmd->buttons & in_duck)
			G::global_cmd->sidemove = sw ? 3.25f : -3.25f;
		else
			G::global_cmd->sidemove = sw ? 2.0 : -2.0;
		sw = !sw;
	}
}

void features::legit_antiaim::resolve()
{
	if (!I::engine_client->in_game_connected() || !c_config::misc::legit_resolver && !c_config::rage::bruteforce) return;

	auto local = I::client_ent_list->get_client_entity(I::engine_client->get_local_player());
	if (!local) return;
	if (!local->is_alive()) return;

	for (int i = 0; i <= I::global_vars->max_clients; i++)
	{
		auto ent = I::client_ent_list->get_client_entity(i);
		if (!ent || ent == local) continue;
		if (!ent->is_alive() || ent->dormant() || ent->get_client_class()->class_id != ClassId_CCSPlayer) continue;

		player_info_t info;
		I::engine_client->get_player_info(i, &info);
		if (info.fakeplayer) return;

		auto animstate = ent->get_anim_state();
		if (!animstate) continue;
		
		auto lby = ent->lby();
		auto lby_delta = lby - ent->eye_angles().y;
		auto abs_lby_delta = std::abs(global_math::normalize_yaw(lby_delta));

		float vel = ent->velocity().Length2D();
		bool statics = false;
		if (vel > 0.0f && vel <= 5.0f) // yes, that is how you should detect static desync, fuck this animlayer shit
			statics = true;

		c_config::visuals::resolved_real = animstate->m_flGoalFeetYaw; // some kind of brain damage

		if (abs_lby_delta >= 35.0f && animstate->speed_2d < 0.1f)
		{
			float desync_delta = ent->max_desync();
			c_config::visuals::resolved_real = ent->eye_angles().y;
			if (lby_delta > 0)
				c_config::visuals::resolved_real += statics ? desync_delta : -desync_delta;
			else if (lby_delta < 0)
				c_config::visuals::resolved_real -= statics ? desync_delta : -desync_delta;
		}

		if (c_config::rage::bruteforce) // i cant stop laughing
		{
			int missed = G::ShotsFired[ent->ent_index()] - G::ShotsHit[ent->ent_index()]; // cancer
			switch (missed % 5)
			{
			case 0:
				if (abs_lby_delta >= 35.0f && animstate->speed_2d < 0.1f)
				{
					float desync_delta = ent->max_desync();
					c_config::visuals::resolved_real = ent->eye_angles().y;
					if (lby_delta > 0)
						c_config::visuals::resolved_real += statics ? desync_delta : -desync_delta;
					else if (lby_delta < 0)
						c_config::visuals::resolved_real -= statics ? desync_delta : -desync_delta;
				}

				break;
			case 1: c_config::visuals::resolved_real = ent->eye_angles().y + 58.f; break;
			case 2: c_config::visuals::resolved_real = ent->eye_angles().y - 58.f; break;
			case 3: c_config::visuals::resolved_real = ent->eye_angles().y + 30.f; break;
			case 4: c_config::visuals::resolved_real = ent->eye_angles().y - 30.f; break;
			}
		}


		global_math::normalize_yaw(c_config::visuals::resolved_real);
		animstate->m_flGoalFeetYaw = c_config::visuals::resolved_real;
	}
}