#pragma once

#include "../sdk/i_surface.h"

#include <vector>
#include <string>

namespace c_config {
	void save(int);
	void load(int);
	void create(const char*);
	void del(int);
	void init();

	inline std::vector<std::string> configs;

	typedef struct _bind {
		_bind(int mode = 0)
		{
			this->mode = mode;
		}

		int key = 0;
		int mode;
		bool state = 0;
		bool opened_parameters = false;
	} bind, *pbind;

	namespace rage {
		enum hitboxes {
			head = 0,
			neck,
			body,
			arms,
			legs
		};

		typedef struct _antiaims {
			int yaw = 0;
			int pitch = 0;
		};

		typedef struct _weapons {
			int hitchance = 0;
			int mindmg = 0;
			bool hitboxes[5] = { 0, 0, 0, 0, 0 };
			int head_scale = 100;
			int body_scale = 100;
		};

		inline bool enabled = false;
		inline bool autofire = false;
		inline bool autostop = false;
		inline bool autoscope = false;
		inline bool bruteforce = false;
		inline int fakelag = 0;
		inline _weapons weapons[6];

		inline bool antiaims_enabled = false;
		inline bool desync = false;
		inline bind manual_back(1);
		inline bind manual_left(1);
		inline bind manual_right(1);
		inline _antiaims antiaims[3];
	}

	namespace legit {
		typedef struct _weapons {
			float fov = 0.0f;
			float silent_fov = 0.0f;
			float rcs_fov = 0.0f;
			float smooth = 1.0f;
			float rcs_smooth = 1.0f;
			int rcs_start_bullet = 2;
			bool rcs_standalone = false;
			float rcs_x = 0.0f;
			float rcs_y = 0.0f;
			int kill_delay = 0;
			int hitbox = 0;
			bool legit_backtrack = false;
			bool only_scoped = false;
			bool autopistol = false;
			int hitchance = 0;
		};

		enum hitboxes {
			nearest = 0,
			head,
			neck,
			sternum,
			chest,
			stomach,
			pelvis
		};

		inline bool legit_aimbot = false;
		inline bool disable_if_flashed = false;
		inline bool disable_in_smoke = false;
		inline bool attack_teammates = false;
		inline bool visible_check = true;
		inline bool auto_pick = false;
		inline bind autofire(0);
		inline bool autostop = false;
		inline int CWeapon = 0;
		inline _weapons weapons[60];
	}

	namespace triggerbot {
		typedef struct _weapons {
			bool enabled = false;
			bool only_scoped = false;
			int hitchance = false;
			bool hitboxes[7] = { 0, 0, 0, 0, 0, 0, 0 };
		};

		inline bool disable_in_smoke = false;
		inline bool disable_if_flashed = false;
		inline bind key(0);
		inline _weapons weapons[60];
	}

	namespace visuals {
		inline bool esp_boxes = false;
		inline bool esp_teammates = false;
		inline color esp_box_colour = color(127.f, 193.f, 255.f, 255.f);
		inline bool esp_names = false;
		inline color esp_name_colour = color(255.f, 255.f, 255.f, 255.f);
		inline bool esp_health = false;
		inline bool esp_health_bar = false;
		inline color esp_health_colour = color(255.f, 255.f, 255.f, 255.f);
		inline bool esp_armour = false;
		inline color esp_armour_colour = color(255.f, 255.f, 255.f, 255.f);
		inline bool esp_weapon = false;
		inline color esp_weapon_colour = color(255.f, 255.f, 255.f, 255.f);
		inline bool esp_money = false;
		inline color esp_money_colour = color(255.f, 255.f, 255.f, 255.f);

		inline bool chams = false;
		inline bool chams_teammates = false;
		inline color chams_colour_vis_t = color(255.f, 255.f, 255.f, 255.f);
		inline color chams_colour_vis_e = color(255.f, 255.f, 255.f, 255.f);
		inline color chams_colour_invis_t = color(255.f, 255.f, 255.f, 255.f);
		inline color chams_colour_invis_e = color(255.f, 255.f, 255.f, 255.f);
		inline bool chams_ignore_z = false;
		inline int chams_type = 0;
		inline bool chams_backtrack = false;
		inline color chams_backtrack_color = color(255.f, 255.f, 255.f, 255.f);

		inline bool chams_arms = false;
		inline bool chams_arms_wireframe = false;
		inline int chams_arms_type = 0;
		inline float chams_arms_alpha = 1.0f;
		inline color chams_arms_color = color(255.f, 255.f, 255.f, 255.f);

		inline bool chams_weapons = false;
		inline bool chams_weapons_wireframe = false;
		inline int chams_weapons_type = 0;
		inline float chams_weapons_alpha = 1.0f;
		inline color chams_weapons_color = color(255.f, 255.f, 255.f, 255.f);

		inline int viewmodel_fov = 0;
		inline bool show_binds = false;
		inline bool thirdperson = false;
		inline int thirdperson_dist = 150;
		inline bind thirdperson_key(1);
		inline bool angle_lines = false;
		inline bool wireframe_smoke = false;
		inline bool nade_predict = false;
		inline bool nightmode = false;
		inline bool reveal_ranks = false;
		inline bool show_fov = false;
		inline bool remove_scope = false;

		inline vec3_t real;
		inline vec3_t fake;
		inline float resolved_real = 0.0f;
		inline matrix3x4_t fake_matrix[128];
	}

	namespace skins {
		struct skin {
			int id = 0;
			float wear = 0.00001f;
		};

		inline skin skins[54];
		inline skin knife;
		inline int knife_id = 0;
	}

	namespace misc {
		inline bool bunnyhop = false;
		inline bool autostrafe = false;
		inline bool clantag_changer = false;
		inline bool infinity_duck = false;
		inline bool legit_resolver = false;
		inline bool legit_aa = false;
		inline bind legit_aa_inverter(1);
		inline bool slowwalk = false;
		inline float slowwalk_speed = 0.0f;
		inline bool svpure_bypass = false;
	}
}