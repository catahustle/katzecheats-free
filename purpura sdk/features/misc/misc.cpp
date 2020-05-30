#include "misc.h"

#include "../../interfaces/interfaces.h"

#include "../../utilities/globals.h"
#include "../../utilities/utilities.h"

#include "../../menu/settings.h"

void features::miscellaneous::bunnyhop(c_usercmd * cmd) {
	if (!c_config::misc::bunnyhop)
		return;

	if (global_variables::local_player->move_type() == move_types::ladder)
		return;

	static bool last_jumped = false;
	static bool should_fake = false;

	if (!last_jumped && should_fake) {
		should_fake = false;
		cmd->buttons |= in_jump;
	}
	else if (cmd->buttons & in_jump) {
		if (global_variables::local_player->flags() & fl_onground)
			should_fake = last_jumped = true;
		else {
			cmd->buttons &= ~in_jump;
			last_jumped = false;
		}
	}
	else should_fake = last_jumped = false;
}

void features::miscellaneous::clantag_spam() {
	if (!I::engine_client->in_game_connected() || !G::local_player) return;

	static bool set = false;

	if (!c_config::misc::clantag_changer)
	{
		if(set) global_utils::set_clantag("", "");
		set = false;
		return;
	}

	set = true;

	static std::string cur_clantag = " katzecheats " ;
	static float oldtime;

	if (interfaces::global_vars->curtime - oldtime >= 0.5f) {
		std::string temp = (std::string&)cur_clantag;
		cur_clantag.erase(0, 1);
		cur_clantag += temp[0];
		global_utils::set_clantag(cur_clantag.c_str(), "katzecheats");
		oldtime = interfaces::global_vars->curtime;
	}
}

void features::miscellaneous::nade_predict()
{
	if (!I::engine_client->in_game_connected() || !G::local_player) return;
	if (!G::local_player->is_alive()) return;

	static bool last = c_config::visuals::nade_predict;
	static auto cvar = I::game_cvar->find_var("cl_grenadepreview");
	if (!cvar) return;

	if (last != c_config::visuals::nade_predict || !cvar->get_bool() != c_config::visuals::nade_predict)
	{
		cvar->set_value(c_config::visuals::nade_predict);
		last = c_config::visuals::nade_predict;
	}
}

void features::miscellaneous::autostrafe()
{
	if (!c_config::misc::autostrafe) return;
	if (!I::engine_client->in_game_connected() || !G::local_player) return;
	if (!G::local_player->is_alive() || G::local_player->move_type() == move_types::ladder || G::local_player->move_type() == move_types::noclip) return;
	if (G::local_player->flags() & fl_onground) return;

	if (G::global_cmd->mousedx < 0)
		G::global_cmd->sidemove = -450.0f;
	else if (G::global_cmd->mousedx > 0)
		G::global_cmd->sidemove = 450.0f;
}

void features::miscellaneous::on_create_move(c_usercmd* cmd) {
	this->bunnyhop(cmd);
	this->autostrafe();
	this->clantag_spam();
	this->nade_predict();
}
