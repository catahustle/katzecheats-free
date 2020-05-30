#include "settings.h"

#include <Windows.h>
#include <ShlObj.h>
#include <fstream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;
using std::ifstream;
using std::ofstream;

namespace c_config
{
	std::string folder;
	void save(int idx)
	{
		json f;

		f["rage"]["enabled"] = rage::enabled;
		f["rage"]["autofire"] = rage::autofire;
		f["rage"]["autostop"] = rage::autostop;
		f["rage"]["autoscope"] = rage::autoscope;
		f["rage"]["bruteforce"] = rage::bruteforce;

		for (int i = 0; i < 6; i++)
		{
			f["rage"][std::to_string(i)]["hitchance"] = rage::weapons[i].hitchance;
			f["rage"][std::to_string(i)]["mindmg"] = rage::weapons[i].mindmg;
			f["rage"][std::to_string(i)]["body_scale"] = rage::weapons[i].body_scale;
			f["rage"][std::to_string(i)]["head_scale"] = rage::weapons[i].head_scale;
			for (int j = 0; j < 5; j++)
				f["rage"][std::to_string(i)][std::to_string(j)]["hitbox"] = rage::weapons[i].hitboxes[j];
		}

		f["rage"]["antiaims_enabled"] = rage::antiaims_enabled;
		f["rage"]["desync"] = rage::desync;
		f["rage"]["fakelag"] = rage::fakelag;
		f["rage"]["manual_back"]["key"] = rage::manual_back.key;
		f["rage"]["manual_back"]["mode"] = rage::manual_back.mode;
		f["rage"]["manual_left"]["key"] = rage::manual_left.key;
		f["rage"]["manual_left"]["mode"] = rage::manual_left.mode;
		f["rage"]["manual_right"]["key"] = rage::manual_right.key;
		f["rage"]["manual_right"]["mode"] = rage::manual_right.mode;

		for (int i = 0; i < 3; i++)
		{
			f["rage"][std::to_string(i)]["pitch"] = rage::antiaims[i].pitch;
			f["rage"][std::to_string(i)]["yaw"] = rage::antiaims[i].yaw;
		}
		
		f["aimbot"]["legit_aimbot"] = legit::legit_aimbot;
		f["aimbot"]["disable_if_flashed"] = legit::disable_if_flashed;
		f["aimbot"]["disable_in_smoke"] = legit::disable_in_smoke;
		f["aimbot"]["attack_teammates"] = legit::attack_teammates;
		f["aimbot"]["visible_check"] = legit::visible_check;
		f["aimbot"]["auto_pick"] = legit::auto_pick;
		f["aimbot"]["autofire"]["key"] = legit::autofire.key;
		f["aimbot"]["autofire"]["mode"] = legit::autofire.mode;
		f["aimbot"]["autostop"] = legit::autostop;

		for (int i = 0; i < 35; i++)
		{
			f["aimbot"][std::to_string(i)]["fov"] = legit::weapons[i].fov;
			f["aimbot"][std::to_string(i)]["silent_fov"] = legit::weapons[i].silent_fov;
			f["aimbot"][std::to_string(i)]["rcs_fov"] = legit::weapons[i].rcs_fov;
			f["aimbot"][std::to_string(i)]["smooth"] = legit::weapons[i].smooth;
			f["aimbot"][std::to_string(i)]["rcs_smooth"] = legit::weapons[i].rcs_smooth;
			f["aimbot"][std::to_string(i)]["rcs_start_bullet"] = legit::weapons[i].rcs_start_bullet;
			f["aimbot"][std::to_string(i)]["rcs_standalone"] = legit::weapons[i].rcs_standalone;
			f["aimbot"][std::to_string(i)]["rcs_x"] = legit::weapons[i].rcs_x;
			f["aimbot"][std::to_string(i)]["rcs_y"] = legit::weapons[i].rcs_y;
			f["aimbot"][std::to_string(i)]["kill_delay"] = legit::weapons[i].kill_delay;
			f["aimbot"][std::to_string(i)]["hitbox"] = legit::weapons[i].hitbox;
			f["aimbot"][std::to_string(i)]["legit_backtrack"] = legit::weapons[i].legit_backtrack;
			f["aimbot"][std::to_string(i)]["only_scoped"] = legit::weapons[i].only_scoped;
			f["aimbot"][std::to_string(i)]["autopistol"] = legit::weapons[i].autopistol;
			f["aimbot"][std::to_string(i)]["hitchance"] = legit::weapons[i].hitchance;
		}

		f["triggerbot"]["disable_in_smoke"] = triggerbot::disable_in_smoke;
		f["triggerbot"]["disable_if_flashed"] = triggerbot::disable_if_flashed;
		f["triggerbot"]["key"]["key"] = triggerbot::key.key;
		f["triggerbot"]["key"]["mode"] = triggerbot::key.mode;
		
		for (int i = 0; i < 35; i++)
		{
			f["triggerbot"][std::to_string(i)]["enabled"] = triggerbot::weapons[i].enabled;
			f["triggerbot"][std::to_string(i)]["only_scoped"] = triggerbot::weapons[i].only_scoped;
			for (int j = 0; j < 7; j++)
				f["triggerbot"][std::to_string(i)][std::to_string(j)]["hitboxes"] = triggerbot::weapons[i].hitboxes[j];
			f["triggerbot"][std::to_string(i)]["hitchance"] = triggerbot::weapons[i].hitchance;
		}

		f["visuals"]["esp_boxes"] = visuals::esp_boxes;
		f["visuals"]["esp_teammates"] = visuals::esp_teammates;

		f["visuals"]["esp_box_colour"]["r"] = visuals::esp_box_colour.r;
		f["visuals"]["esp_box_colour"]["g"] = visuals::esp_box_colour.g;
		f["visuals"]["esp_box_colour"]["b"] = visuals::esp_box_colour.b;

		f["visuals"]["esp_names"] = visuals::esp_names;
		f["visuals"]["esp_health"] = visuals::esp_health;
		f["visuals"]["esp_health_bar"] = visuals::esp_health_bar;
		f["visuals"]["esp_armour"] = visuals::esp_armour;
		f["visuals"]["esp_weapon"] = visuals::esp_weapon;
		f["visuals"]["esp_money"] = visuals::esp_money;
		f["visuals"]["chams"] = visuals::chams;
		f["visuals"]["chams_teammates"] = visuals::chams_teammates;

		f["visuals"]["chams_colour_vis_t"]["r"] = visuals::chams_colour_vis_t.r;
		f["visuals"]["chams_colour_vis_t"]["g"] = visuals::chams_colour_vis_t.g;
		f["visuals"]["chams_colour_vis_t"]["b"] = visuals::chams_colour_vis_t.b;

		f["visuals"]["chams_colour_vis_e"]["r"] = visuals::chams_colour_vis_e.r;
		f["visuals"]["chams_colour_vis_e"]["g"] = visuals::chams_colour_vis_e.g;
		f["visuals"]["chams_colour_vis_e"]["b"] = visuals::chams_colour_vis_e.b;

		f["visuals"]["chams_colour_invis_t"]["r"] = visuals::chams_colour_invis_t.r;
		f["visuals"]["chams_colour_invis_t"]["g"] = visuals::chams_colour_invis_t.g;
		f["visuals"]["chams_colour_invis_t"]["b"] = visuals::chams_colour_invis_t.b;

		f["visuals"]["chams_colour_invis_e"]["r"] = visuals::chams_colour_invis_e.r;
		f["visuals"]["chams_colour_invis_e"]["g"] = visuals::chams_colour_invis_e.g;
		f["visuals"]["chams_colour_invis_e"]["b"] = visuals::chams_colour_invis_e.b;

		f["visuals"]["chams_backtrack_color"]["r"] = visuals::chams_backtrack_color.r;
		f["visuals"]["chams_backtrack_color"]["g"] = visuals::chams_backtrack_color.g;
		f["visuals"]["chams_backtrack_color"]["b"] = visuals::chams_backtrack_color.b;

		f["visuals"]["chams_arms_color"]["r"] = visuals::chams_arms_color.r;
		f["visuals"]["chams_arms_color"]["g"] = visuals::chams_arms_color.g;
		f["visuals"]["chams_arms_color"]["b"] = visuals::chams_arms_color.b;

		f["visuals"]["chams_weapons_color"]["r"] = visuals::chams_weapons_color.r;
		f["visuals"]["chams_weapons_color"]["g"] = visuals::chams_weapons_color.g;
		f["visuals"]["chams_weapons_color"]["b"] = visuals::chams_weapons_color.b;

		f["visuals"]["chams_ignore_z"] = visuals::chams_ignore_z;
		f["visuals"]["chams_type"] = visuals::chams_type;
		f["visuals"]["chams_backtrack"] = visuals::chams_backtrack;
		f["visuals"]["chams_arms"] = visuals::chams_arms;
		f["visuals"]["chams_arms_wireframe"] = visuals::chams_arms_wireframe;
		f["visuals"]["chams_arms_type"] = visuals::chams_arms_type;
		f["visuals"]["chams_arms_alpha"] = visuals::chams_arms_alpha;
		f["visuals"]["chams_weapons"] = visuals::chams_weapons;
		f["visuals"]["chams_weapons_wireframe"] = visuals::chams_weapons_wireframe;
		f["visuals"]["chams_weapons_type"] = visuals::chams_weapons_type;
		f["visuals"]["chams_weapons_alpha"] = visuals::chams_weapons_alpha;
		f["visuals"]["viewmodel_fov"] = visuals::viewmodel_fov;
		f["visuals"]["show_binds"] = visuals::show_binds;
		f["visuals"]["thirdperson"] = visuals::thirdperson;
		f["visuals"]["thirdperson_dist"] = visuals::thirdperson_dist;
		f["visuals"]["thirdperson_key"]["key"] = visuals::thirdperson_key.key;
		f["visuals"]["thirdperson_key"]["mode"] = visuals::thirdperson_key.mode;
		f["visuals"]["angle_lines"] = visuals::angle_lines;
		f["visuals"]["wireframe_smoke"] = visuals::wireframe_smoke;
		f["visuals"]["nade_predict"] = visuals::nade_predict;
		f["visuals"]["nightmode"] = visuals::nightmode;
		f["visuals"]["reveal_ranks"] = visuals::reveal_ranks;
		f["visuals"]["show_fov"] = visuals::show_fov;
		f["visuals"]["remove_scope"] = visuals::remove_scope;

		for (int i = 0; i < 35; i++)
		{
			f["skins"]["skins"][std::to_string(i)]["id"] = skins::skins[i].id;
			f["skins"]["skins"][std::to_string(i)]["wear"] = skins::skins[i].wear;
		}

		f["skins"]["knife"]["id"] = skins::knife.id;
		f["skins"]["knife"]["wear"] = skins::knife.wear;
		f["skins"]["knife_id"] = skins::knife_id;

		f["misc"]["bunnyhop"] = misc::bunnyhop;
		f["misc"]["autostrafe"] = misc::autostrafe;
		f["misc"]["clantag_changer"] = misc::clantag_changer;
		f["misc"]["infinity_duck"] = misc::infinity_duck;
		f["misc"]["legit_aa"] = misc::legit_aa;
		f["misc"]["legit_aa_inverter"]["key"] = misc::legit_aa_inverter.key;
		f["misc"]["legit_aa_inverter"]["mode"] = misc::legit_aa_inverter.mode;
		f["misc"]["slowwalk"] = misc::slowwalk;
		f["misc"]["slowwalk_speed"] = misc::slowwalk_speed;
		f["misc"]["legit_resolver"] = misc::legit_resolver;
		f["misc"]["svpure_bypass"] = misc::svpure_bypass;
		f["misc"]["infinity_duck"] = misc::infinity_duck;

		std::string file = folder + configs[idx];
		ofstream f1(file);
		f1 << f;
		f1.close();
	}

	void load(int idx)
	{
		json f;
		std::string file = folder + configs[idx];
		ifstream f1(file);
		f1 >> f;
		f1.close();

		if (!f["rage"].is_null())
		{
			rage::enabled = f["rage"]["enabled"];
			rage::autofire = f["rage"]["autofire"];
			rage::autostop = f["rage"]["autostop"];
			rage::autoscope = f["rage"]["autoscope"];
			if(!f["rage"]["bruteforce"].is_null()) rage::bruteforce = f["rage"]["bruteforce"];
			if(!f["rage"]["fakelag"].is_null()) rage::fakelag = f["rage"]["fakelag"];

			for (int i = 0; i < 6; i++)
			{
				rage::weapons[i].hitchance = f["rage"][std::to_string(i)]["hitchance"];
				rage::weapons[i].mindmg = f["rage"][std::to_string(i)]["mindmg"];
				rage::weapons[i].body_scale = f["rage"][std::to_string(i)]["body_scale"];
				rage::weapons[i].head_scale = f["rage"][std::to_string(i)]["head_scale"];
				for (int j = 0; j < 5; j++)
					rage::weapons[i].hitboxes[j] = f["rage"][std::to_string(i)][std::to_string(j)]["hitbox"];
			}

			if (!f["rage"]["antiaims_enabled"].is_null())
			{
				rage::antiaims_enabled = f["rage"]["antiaims_enabled"];
				rage::desync = f["rage"]["desync"];
				rage::manual_back.key = f["rage"]["manual_back"]["key"];
				rage::manual_back.mode = f["rage"]["manual_back"]["mode"];
				rage::manual_left.key = f["rage"]["manual_left"]["key"];
				rage::manual_left.mode = f["rage"]["manual_left"]["mode"];
				rage::manual_right.key = f["rage"]["manual_right"]["key"];
				rage::manual_right.mode = f["rage"]["manual_right"]["mode"];

				for (int i = 0; i < 3; i++)
				{
					rage::antiaims[i].pitch = f["rage"][std::to_string(i)]["pitch"];
					rage::antiaims[i].yaw = f["rage"][std::to_string(i)]["yaw"];
				}
			}
		}

		legit::disable_if_flashed = f["aimbot"]["disable_if_flashed"];
		legit::disable_in_smoke = f["aimbot"]["disable_in_smoke"];
		legit::attack_teammates = f["aimbot"]["attack_teammates"];
		legit::visible_check = f["aimbot"]["visible_check"];
		legit::auto_pick = f["aimbot"]["auto_pick"];
		legit::legit_aimbot = f["aimbot"]["legit_aimbot"];
		if (!f["aimbot"]["autofire"].is_null())
		{
			if(!f["aimbot"]["autofire"]["key"].is_null()) legit::autofire.key = f["aimbot"]["autofire"]["key"];
			if(!f["aimbot"]["autofire"]["mode"].is_null()) legit::autofire.mode = f["aimbot"]["autofire"]["mode"];
		}
		if (!f["aimbot"]["autostop"].is_null()) legit::autostop = f["aimbot"]["autostop"];
		
		for (int i = 0; i < 35; i++)
		{
			legit::weapons[i].fov = f["aimbot"][std::to_string(i)]["fov"];
			legit::weapons[i].silent_fov = f["aimbot"][std::to_string(i)]["silent_fov"];
			legit::weapons[i].rcs_fov = f["aimbot"][std::to_string(i)]["rcs_fov"];
			legit::weapons[i].smooth = f["aimbot"][std::to_string(i)]["smooth"];
			legit::weapons[i].rcs_smooth = f["aimbot"][std::to_string(i)]["rcs_smooth"];
			legit::weapons[i].rcs_start_bullet = f["aimbot"][std::to_string(i)]["rcs_start_bullet"];
			legit::weapons[i].rcs_standalone = f["aimbot"][std::to_string(i)]["rcs_standalone"];
			legit::weapons[i].rcs_x = f["aimbot"][std::to_string(i)]["rcs_x"];
			legit::weapons[i].rcs_y = f["aimbot"][std::to_string(i)]["rcs_y"];
			legit::weapons[i].kill_delay = f["aimbot"][std::to_string(i)]["kill_delay"];
			legit::weapons[i].hitbox = f["aimbot"][std::to_string(i)]["hitbox"];
			legit::weapons[i].legit_backtrack = f["aimbot"][std::to_string(i)]["legit_backtrack"];
			legit::weapons[i].only_scoped = f["aimbot"][std::to_string(i)]["only_scoped"];
			legit::weapons[i].autopistol = f["aimbot"][std::to_string(i)]["autopistol"];
			if (!f["aimbot"][std::to_string(i)]["hitchance"].is_null()) legit::weapons[i].hitchance = f["aimbot"][std::to_string(i)]["hitchance"];
		}		

		triggerbot::disable_in_smoke = f["triggerbot"]["disable_in_smoke"];
		triggerbot::disable_if_flashed = f["triggerbot"]["disable_if_flashed"];
		triggerbot::key.key = f["triggerbot"]["key"]["key"];
		triggerbot::key.mode = f["triggerbot"]["key"]["mode"];

		for (int i = 0; i < 35; i++)
		{
			triggerbot::weapons[i].enabled = f["triggerbot"][std::to_string(i)]["enabled"];
			triggerbot::weapons[i].only_scoped = f["triggerbot"][std::to_string(i)]["only_scoped"];
			for (int j = 0; j < 7; j++)
				triggerbot::weapons[i].hitboxes[j] = f["triggerbot"][std::to_string(i)][std::to_string(j)]["hitboxes"];
			if (!f["triggerbot"][std::to_string(i)]["hitchance"].is_null()) triggerbot::weapons[i].hitchance = f["triggerbot"][std::to_string(i)]["hitchance"];
		}

		visuals::esp_boxes = f["visuals"]["esp_boxes"];
		visuals::esp_teammates = f["visuals"]["esp_teammates"];

		visuals::esp_box_colour.r = f["visuals"]["esp_box_colour"]["r"];
		visuals::esp_box_colour.g = f["visuals"]["esp_box_colour"]["g"];
		visuals::esp_box_colour.b = f["visuals"]["esp_box_colour"]["b"];

		visuals::esp_names = f["visuals"]["esp_names"];
		visuals::esp_health = f["visuals"]["esp_health"];
		visuals::esp_health_bar = f["visuals"]["esp_health_bar"];
		visuals::esp_armour = f["visuals"]["esp_armour"];
		visuals::esp_weapon = f["visuals"]["esp_weapon"];
		visuals::esp_money = f["visuals"]["esp_money"];
		visuals::chams = f["visuals"]["chams"];
		visuals::chams_teammates = f["visuals"]["chams_teammates"];

		visuals::chams_colour_vis_t.r = f["visuals"]["chams_colour_vis_t"]["r"];
		visuals::chams_colour_vis_t.g = f["visuals"]["chams_colour_vis_t"]["g"];
		visuals::chams_colour_vis_t.b = f["visuals"]["chams_colour_vis_t"]["b"];

		visuals::chams_colour_vis_e.r = f["visuals"]["chams_colour_vis_e"]["r"];
		visuals::chams_colour_vis_e.g = f["visuals"]["chams_colour_vis_e"]["g"];
		visuals::chams_colour_vis_e.b = f["visuals"]["chams_colour_vis_e"]["b"];

		visuals::chams_colour_invis_t.r = f["visuals"]["chams_colour_invis_t"]["r"];
		visuals::chams_colour_invis_t.g = f["visuals"]["chams_colour_invis_t"]["g"];
		visuals::chams_colour_invis_t.b = f["visuals"]["chams_colour_invis_t"]["b"];

		visuals::chams_colour_invis_e.r = f["visuals"]["chams_colour_invis_e"]["r"];
		visuals::chams_colour_invis_e.g = f["visuals"]["chams_colour_invis_e"]["g"];
		visuals::chams_colour_invis_e.b = f["visuals"]["chams_colour_invis_e"]["b"];

		visuals::chams_backtrack_color.r = f["visuals"]["chams_backtrack_color"]["r"];
		visuals::chams_backtrack_color.g = f["visuals"]["chams_backtrack_color"]["g"];
		visuals::chams_backtrack_color.b = f["visuals"]["chams_backtrack_color"]["b"];

		visuals::chams_arms_color.r = f["visuals"]["chams_arms_color"]["r"];
		visuals::chams_arms_color.g = f["visuals"]["chams_arms_color"]["g"];
		visuals::chams_arms_color.b = f["visuals"]["chams_arms_color"]["b"];

		visuals::chams_weapons_color.r = f["visuals"]["chams_weapons_color"]["r"];
		visuals::chams_weapons_color.g = f["visuals"]["chams_weapons_color"]["g"];
		visuals::chams_weapons_color.b = f["visuals"]["chams_weapons_color"]["b"];

		visuals::chams_ignore_z = f["visuals"]["chams_ignore_z"];
		visuals::chams_type = f["visuals"]["chams_type"];
		visuals::chams_backtrack = f["visuals"]["chams_backtrack"];
		visuals::chams_arms = f["visuals"]["chams_arms"];
		visuals::chams_arms_wireframe = f["visuals"]["chams_arms_wireframe"];
		visuals::chams_arms_type = f["visuals"]["chams_arms_type"];
		visuals::chams_arms_alpha = f["visuals"]["chams_arms_alpha"];
		visuals::chams_weapons = f["visuals"]["chams_weapons"];
		visuals::chams_weapons_wireframe = f["visuals"]["chams_weapons_wireframe"];
		visuals::chams_weapons_type = f["visuals"]["chams_weapons_type"];
		visuals::chams_weapons_alpha = f["visuals"]["chams_weapons_alpha"];
		visuals::viewmodel_fov = f["visuals"]["viewmodel_fov"];
		visuals::show_binds = f["visuals"]["show_binds"];
		visuals::thirdperson = f["visuals"]["thirdperson"];
		visuals::thirdperson_dist = f["visuals"]["thirdperson_dist"];
		visuals::thirdperson_key.key = f["visuals"]["thirdperson_key"]["key"];
		visuals::thirdperson_key.mode = f["visuals"]["thirdperson_key"]["mode"];
		visuals::angle_lines = f["visuals"]["angle_lines"];
		visuals::wireframe_smoke = f["visuals"]["wireframe_smoke"];
		visuals::nade_predict = f["visuals"]["nade_predict"];
		visuals::nightmode = f["visuals"]["nightmode"];
		visuals::reveal_ranks = f["visuals"]["reveal_ranks"];
		visuals::show_fov = f["visuals"]["show_fov"];
		if(!f["visuals"]["remove_scope"].is_null()) visuals::remove_scope = f["visuals"]["remove_scope"];

		for (int i = 0; i < 35; i++)
		{
			skins::skins[i].id = f["skins"]["skins"][std::to_string(i)]["id"];
			skins::skins[i].wear = f["skins"]["skins"][std::to_string(i)]["wear"];
		}

		skins::knife.id = f["skins"]["knife"]["id"];
		skins::knife.wear = f["skins"]["knife"]["wear"];
		skins::knife_id = f["skins"]["knife_id"];

		misc::bunnyhop = f["misc"]["bunnyhop"];
		misc::autostrafe = f["misc"]["autostrafe"];
		misc::clantag_changer = f["misc"]["clantag_changer"];
		misc::infinity_duck = f["misc"]["infinity_duck"];
		misc::legit_aa = f["misc"]["legit_aa"];
		misc::legit_aa_inverter.key = f["misc"]["legit_aa_inverter"]["key"];
		misc::legit_aa_inverter.mode = f["misc"]["legit_aa_inverter"]["mode"];
		if (!f["misc"]["slowwalk"].is_null()) misc::slowwalk = f["misc"]["slowwalk"];
		if (!f["misc"]["legit_resolver"].is_null()) misc::legit_resolver = f["misc"]["legit_resolver"];
		if (!f["misc"]["slowwalk_speed"].is_null()) misc::slowwalk_speed = f["misc"]["slowwalk_speed"];
		if (!f["misc"]["svpure_bypass"].is_null()) misc::svpure_bypass = f["misc"]["svpure_bypass"];
		if (!f["misc"]["infinity_duck"].is_null()) misc::infinity_duck = f["misc"]["infinity_duck"];
	}

	void init()
	{
		configs.clear();

		CHAR path[260];
		GetModuleFileNameEx(GetCurrentProcess(), 0, path, 260);
		std::string szPath = path;
		szPath.erase(szPath.length() - 9);
		szPath += "\\KatzeCheats";
		folder = szPath + "\\";
		CreateDirectoryA(szPath.c_str(), 0);
		WIN32_FIND_DATA FindFileData;
		HANDLE hf;
		std::string szPath2 = szPath + "\\default.json";
		szPath += "\\*.json";
		hf = FindFirstFileA(szPath.c_str(), &FindFileData);
		if (hf != INVALID_HANDLE_VALUE)
		{
			do
			{
				configs.push_back(FindFileData.cFileName);

			} while (FindNextFileA(hf, &FindFileData) != 0);
			FindClose(hf);
		}

		if (!configs.size())
		{
			HANDLE hFile = CreateFileA(szPath2.c_str(), GENERIC_WRITE, 0, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
			if (hFile) CloseHandle(hFile);
		}
	}

	void create(const char* name)
	{
		std::string finalname = name;
		if (finalname.find(".json") == std::string::npos) finalname += ".json";
		std::string szPath = folder + finalname;
		HANDLE hFile = CreateFileA(szPath.c_str(), GENERIC_WRITE, 0, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
		if (hFile) CloseHandle(hFile);

		configs.push_back(finalname);
		save(configs.size() - 1);
	}

	void del(int idx)
	{
		std::string szPath = folder + configs[idx];
		DeleteFileA(szPath.c_str());
		init();
	}
}