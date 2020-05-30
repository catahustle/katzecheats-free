#pragma once

#include <array>

#include "../utilities/netvar_manager.h"
#include "../utilities/utilities.h"

#include "i_client_entity.h"

#include "recv.h"
#include "client_class.h"
#include "structs.h"
#include "collideable.h"
#include "weapon_definitions.h"

#define bone_hitbox_cool	0x00000100 // 256

enum entity_flags {
	fl_onground = (1 << 0),
	fl_ducking = (1 << 1),
	fl_waterjump = (1 << 2),
	fl_ontrain = (1 << 3),
	fl_inrain = (1 << 4),
	fl_frozen = (1 << 5),
	fl_atcontrols = (1 << 6),
	fl_client = (1 << 7),
	fl_fakeclient = (1 << 8),
	fl_inwater = (1 << 9),
	fl_fly = (1 << 10),
	fl_swim = (1 << 11),
	fl_conveyor = (1 << 12),
	fl_npc = (1 << 13),
	fl_godmode = (1 << 14),
	fl_notarget = (1 << 15),
	fl_aimtarget = (1 << 16),
	fl_partialground = (1 << 17),
	fl_staticprop = (1 << 18),
	fl_graphed = (1 << 19),
	fl_grenade = (1 << 20),
	fl_stepmovement = (1 << 21),
	fl_donttouch = (1 << 22),
	fl_basevelocity = (1 << 23),
	fl_worldbrush = (1 << 24),
	fl_object = (1 << 25),
	fl_killme = (1 << 26),
	fl_onfire = (1 << 27),
	fl_dissolving = (1 << 28),
	fl_transragdoll = (1 << 29),
	fl_unblockable_by_player = (1 << 30)
};

struct animstate_pose_param_cache_t {
	std::uint8_t pad_0x0[0x4]; //0x0
	std::uint32_t m_idx; //0x4 
	char* m_name; //0x8 
};

struct c_anim_state
{
	void* pThis;
	char pad2[91];
	void* pBaseEntity; //0x60
	void* pActiveWeapon; //0x64
	void* pLastActiveWeapon; //0x68
	float m_flLastClientSideAnimationUpdateTime; //0x6C
	int m_iLastClientSideAnimationUpdateFramecount; //0x70
	float m_flEyePitch; //0x74
	float m_flEyeYaw; //0x78
	float m_flPitch; //0x7C
	float m_flGoalFeetYaw; //0x80
	float m_flCurrentFeetYaw; //0x84
	float m_flCurrentTorsoYaw; //0x88
	float m_flUnknownVelocityLean; //0x8C //changes when moving/jumping/hitting ground
	float m_flLeanAmount; //0x90
	char pad4[4]; //NaN
	float m_flFeetCycle; //0x98 0 to 1
	float m_flFeetYawRate; //0x9C 0 to 1
	float m_fUnknown2;
	float m_fDuckAmount; //0xA4
	float m_fLandingDuckAdditiveSomething; //0xA8
	float m_fUnknown3; //0xAC
	vec3_t m_vOrigin; //0xB0, 0xB4, 0xB8
	vec3_t m_vLastOrigin; //0xBC, 0xC0, 0xC4
	float m_vVelocityX; //0xC8
	float m_vVelocityY; //0xCC
	char pad5[4];
	float m_flUnknownFloat1; //0xD4 Affected by movement and direction
	char pad6[8];
	float m_flUnknownFloat2; //0xE0 //from -1 to 1 when moving and affected by direction
	float m_flUnknownFloat3; //0xE4 //from -1 to 1 when moving and affected by direction
	float m_unknown; //0xE8
	float speed_2d; //0xEC
	float flUpVelocity; //0xF0
	float m_flSpeedNormalized; //0xF4 //from 0 to 1
	float m_flFeetSpeedForwardsOrSideWays; //0xF8 //from 0 to 2. something  is 1 when walking, 2.something when running, 0.653 when crouch walking
	float m_flFeetSpeedUnknownForwardOrSideways; //0xFC //from 0 to 3. something
	float m_flTimeSinceStartedMoving; //0x100
	float m_flTimeSinceStoppedMoving; //0x104
	unsigned char m_bOnGround; //0x108
	unsigned char m_bInHitGroundAnimation; //0x109
	char pad7[10];
	float m_flLastOriginZ; //0x114
	float m_flHeadHeightOrOffsetFromHittingGroundAnimation; //0x118 from 0 to 1, is 1 when standing
	float m_flGroundFraction; //0x11C from 0 to 1, doesnt change when walking or crouching, only running
	char pad8[4]; //NaN
	float m_flDuckFraction; //0x124 affected while jumping and running, or when just jumping, 0 to 1
	std::uint8_t pad_0x0128[0xC]; //0x0128
	bool m_moving; //0x0134
	std::uint8_t pad_0x0135[0x7B]; //0x0135
	animstate_pose_param_cache_t m_lean_yaw_pose; //0x1B0
	animstate_pose_param_cache_t m_speed_pose; //0x01BC
	animstate_pose_param_cache_t m_ladder_speed_pose; //0x01C8
	animstate_pose_param_cache_t m_ladder_yaw_pose; //0x01D4
	animstate_pose_param_cache_t m_move_yaw_pose; //0x01E0
	animstate_pose_param_cache_t m_unk_pose_pose; //0x01EC 
	animstate_pose_param_cache_t m_body_yaw_pose; //0x01F8
	animstate_pose_param_cache_t m_body_pitch_pose; //0x0204
	animstate_pose_param_cache_t m_dead_yaw_pose; //0x0210
	animstate_pose_param_cache_t m_stand_pose; //0x021C
	animstate_pose_param_cache_t m_jump_fall_pose; //0x0228
	animstate_pose_param_cache_t m_aim_blend_stand_idle_pose; //0x0234
	animstate_pose_param_cache_t m_aim_blend_crouch_idle_pose; //0x0240
	animstate_pose_param_cache_t m_strafe_yaw_pose; //0x024C
	animstate_pose_param_cache_t m_aim_blend_stand_walk_pose; //0x0258
	animstate_pose_param_cache_t m_aim_blend_stand_run_pose; //0x0264
	animstate_pose_param_cache_t m_aim_blend_crouch_walk_pose; //0x0270
	animstate_pose_param_cache_t m_move_blend_walk_pose; //0x027C
	animstate_pose_param_cache_t m_move_blend_run_pose; //0x0288
	animstate_pose_param_cache_t m_move_blend_crouch_pose; //0x0294
	std::uint8_t pad_0x02A0[0x4]; //0x02A0
	float m_vel_unk; //0x02A4 
	std::uint8_t pad_0x02A8[0x86]; //0x02A8
	float m_min_yaw; //0x0330 
	float m_max_yaw; //0x0334 
	float m_max_pitch; //0x0338 
	float m_min_pitch; //0x033C
};

class c_animation_layer
{
public:
	char  pad_0000[20];
	uint32_t m_nOrder; //0x0014
	uint32_t m_nSequence; //0x0018
	float_t m_flPrevCycle; //0x001C
	float_t m_flWeight; //0x0020
	float_t m_flWeightDeltaRate; //0x0024
	float_t m_flPlaybackRate; //0x0028
	float_t m_flCycle; //0x002C
	void* m_pOwner; //0x0030 // player's thisptr
	char  pad_0038[4]; //0x0034
}; //Size: 0x0038

class c_base_combat_weapon;

class c_base_entity : public i_client_entity {
private:
	template<class T>
	T get_pointer(const int offset) {
		return reinterpret_cast <T*> (reinterpret_cast <std::uintptr_t> (this) + offset);
	}

	template<class T>
	T get_value(const int offset) {
		return *reinterpret_cast <T*> (reinterpret_cast <std::uintptr_t> (this) + offset);
	}
public:
	// --------------------------- OFFSETS ---------------------------
	void* animating() {
		return reinterpret_cast<void*>(uintptr_t(this) + 0x4);
	}
	void* networkable() {
		return reinterpret_cast<void*>(uintptr_t(this) + 0x8);
	}
	c_client_class* get_client_class() {
		void* __networkable = this->networkable();
		typedef c_client_class*(__thiscall* orig_fn)(void*);
		return global_utils::legacy_v_function<orig_fn>(__networkable, 2)(__networkable);
	}

	int& take_damage()
	{
		return *(int*)((uintptr_t)this + 640);
	}

	bool dormant() {
		if (!(uintptr_t)this) return true;
		return *reinterpret_cast <bool*> (uintptr_t(this) + 0xED);
	}

	move_types move_type() {
		if (!(uintptr_t)this) return move_types::none;
		return get_value < move_types >(netvar_manager->get_offset("DT_BaseEntity", "m_nRenderMode") + 1);
	}

	NETVAR("DT_CSPlayer", "m_flFlashDuration", flash_duration, float);
	NETVAR("DT_CSPlayer", "m_flFlashMaxAlpha", flash_max_alpha, float);
	NETVAR("DT_CSPlayer", "m_vecViewOffset[0]", view_offset, vec3_t);
	NETVAR("DT_BasePlayer", "m_aimPunchAngle", aim_punch_angle, vec3_t);
	NETVAR("DT_CSPlayer", "m_bGunGameImmunity", gun_game_immunity, bool);
	NETVAR("DT_CSPlayer", "m_bIsScoped", scoped, bool);
	NETVAR("DT_CSPlayer", "m_iShotsFired", shots, int);
	NETVAR("DT_CSPlayer", "m_flSimulationTime", simulation_time, float);
	NETVAR("DT_BasePlayer", "m_nTickBase", tickbase, int);
	NETVAR("DT_CSPlayer", "m_iAccount", money, int);
	NETVAR("DT_BasePlayer", "m_flDuckAmount", crouch_ammount, float);
	NETVAR("DT_BaseEntity", "m_vecMaxs", obb_max, vec3_t);
	NETVAR("DT_BaseEntity", "m_vecMins", obb_min, vec3_t);
	NETVAR("DT_BasePlayer", "m_vecOrigin", origin, vec3_t);
	NETVAR("DT_CSPlayer", "m_fFlags", flags, entity_flags);
	NETVAR("DT_CSPlayer", "m_bHasHeavyArmor", heavy_armour, bool);
	NETVAR("DT_CSPlayer", "m_bHasHelmet", helmet, bool);
	NETVAR("DT_CSPlayer", "m_ArmorValue", armour, int);
	NETVAR("DT_BasePlayer", "m_iHealth", health, int);
	NETVAR("DT_CSPlayer", "m_lifeState", life_state, int);
	NETVAR("DT_BasePlayer", "m_iTeamNum", team, int);
	NETVAR("DT_CSPlayer", "m_flLowerBodyYawTarget", lby, float);
	NETVAR("DT_CSPlayer", "m_angEyeAngles", eye_angles, vec3_t);
	NETVAR("DT_CSPlayer", "m_flHealthShotBoostExpirationTime ", healthshotboost, float);
	NETVAR("DT_BasePlayer", "m_vecVelocity[0]", velocity, vec3_t);
	NETVAR("DT_BasePlayer", "m_iObserverMode", m_iObserverMode, int);
	NETVAR("DT_BasePlayer", "m_hViewModel[0]", get_view_model, c_base_handle);
	NETVAR("DT_BaseEntity", "m_nModelIndex", get_model_index, unsigned);

	std::array<c_base_handle, 48>& get_weapons()
	{
		static auto offset = netvar_manager->get_offset("DT_BaseCombatCharacter", "m_hMyWeapons");
		return *reinterpret_cast<std::array<c_base_handle, 48>*>(uintptr_t(this) + offset);
	}

	vec3_t* get_vangles()
	{
		if (!(uintptr_t)this) return nullptr;
		static auto deadflag = netvar_manager->get_offset("DT_CSPlayer", "deadflag");
		return (vec3_t*)((uintptr_t)this + 0x31D8);
	}

	c_base_combat_weapon* active_weapon();
	bool is_visible();

	// --------------------------- OTHERS ---------------------------
	vec3_t& abs_origin() {
		return global_utils::call_v_table<vec3_t&(__thiscall*)(void*)>(this, 10)(this);
	}
	model_t* model() {
		using original_fn = model_t * (__thiscall*)(void*);
		return (*(original_fn**)animating())[8](animating());
	}
	bool is_alive() {
		return this->health() > 0 && !this->life_state();
	}
	int draw_model(int flags, uint8_t alpha) {
		using original_fn = int(__thiscall*)(void*, int, uint8_t);
		return (*(original_fn**)animating())[9](animating(), flags, alpha);
	}
	bool setup_bones(matrix3x4_t* out, int max_bones, int mask, float time) {
		if (!this) return false;
		using original_fn = bool(__thiscall*)(void*, matrix3x4_t*, int, int, float);
		return (*(original_fn**)animating())[13](animating(), out, max_bones, mask, time);
	}
	vec3_t bone_pos(int i) {
		matrix3x4_t bone_matrix[128];
		if (this->setup_bones(bone_matrix, 128, bone_hitbox_cool, GetTickCount64())) {
			return vec3_t(bone_matrix[i][0][3], bone_matrix[i][1][3], bone_matrix[i][2][3]);
		}
		return vec3_t(0, 0, 0);
	}
	vec3_t& abs_angles() {
		using original_fn = vec3_t & (__thiscall*)(void*);
		return (*(original_fn**)this)[11](this);
	}
	int ent_index() {
		using original_fn = int(__thiscall*)(void*);
		return (*(original_fn**)networkable())[10](networkable());
	}
	c_anim_state* get_anim_state()
	{
		return *(c_anim_state**)((DWORD)this + 0x3900);
	}
	float spawntime() {
		static DWORD spawntime = *(DWORD*)(global_utils::get_signature("client_panorama.dll", "89 86 ? ? ? ? E8 ? ? ? ? 80 BE ? ? ? ? ?") + 2);
		return *(float*)((uintptr_t)this + spawntime);
	}
	void reset_anim_state(c_anim_state* state)
	{
		using ResetAnimState_t = void(__thiscall*)(c_anim_state*);
		static auto ResetAnimState = (ResetAnimState_t)global_utils::get_signature("client_panorama.dll", "56 6A 01 68 ? ? ? ? 8B F1");
		if (!ResetAnimState)
			return;

		ResetAnimState(state);
	}
	void create_anim_state(c_anim_state* state)
	{
		using CreateAnimState_t = void(__thiscall*)(c_anim_state*, c_base_entity*);
		static auto CreateAnimState = (CreateAnimState_t)global_utils::get_signature("client_panorama.dll", "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46");
		if (!CreateAnimState)
			return;

		CreateAnimState(state, this);
	}
	void update_anim_state(c_anim_state* state, vec3_t angle)
	{
		static auto UpdateAnimState = global_utils::get_signature(
			"client_panorama.dll", "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24");

		if (!UpdateAnimState)
			return;

		__asm {
			push 0
		}

		__asm
		{
			mov ecx, state

			movss xmm1, dword ptr[angle + 4]
			movss xmm2, dword ptr[angle]

			call UpdateAnimState
		}
	}
	std::array<float, 24>& get_pose_parameters() const {
		static int _m_flPoseParameter = netvar_manager->get_net_var(fnv_hash("DT_BaseAnimating"), fnv_hash("m_flPoseParameter"));
		return *(std::array<float, 24>*)((uintptr_t)this + _m_flPoseParameter);
	}
	c_animation_layer* get_anim_overlays()
	{
		if (!(uintptr_t)this) return nullptr;
		return *(c_animation_layer**)((DWORD)this + 0x2980);
	}
	float old_simulation_time() {
		if (!(uintptr_t)this) return 0.0f;
		static int name = netvar_manager->get_net_var(fnv_hash("DT_BaseEntity"), fnv_hash("m_flSimulationTime"));
		return *(float*)((uintptr_t)this + (name + 4));
	}
	int get_num_anim_overlays()
	{
		if (!(uintptr_t)this) return 0;
		return *(int*)((DWORD)this + 0x298C);
	}

	void set_model_index(int index)
	{
		if (!(uintptr_t)this) return;
		using original_fn = void(__thiscall*)(void*, int);
		(*(original_fn**)this)[75](this, index);
	}

	float max_desync() {

		auto animstate = uintptr_t(this->get_anim_state());

		float rate = 180;
		float duckammount = *(float*)(animstate + 0xA4);
		float speedfraction = max(0, min(*reinterpret_cast<float*>(animstate + 0xF8), 1));

		float speedfactor = max(0, min(1, *reinterpret_cast<float*> (animstate + 0xFC)));

		float unk1 = ((*reinterpret_cast<float*> (animstate + 0x11C) * -0.30000001) - 0.19999999) * speedfraction;
		float unk2 = unk1 + 1.f;
		float unk3;

		if (duckammount > 0) {

			unk2 += ((duckammount * speedfactor) * (0.5f - unk2));

		}

		return *(float*)(animstate + 0x334) * unk2;

	}

	int get_sequence_activity(int seq);
	void invalidate_bone_cache();
};

class c_base_combat_weapon : public c_base_entity {
private:
	template<class T>
	T get_pointer(const int offset) {
		return reinterpret_cast <T*> (reinterpret_cast <std::uintptr_t> (this) + offset);
	}

	template<class T>
	T get_value(const int offset) {
		return *reinterpret_cast <T*> (reinterpret_cast <std::uintptr_t> (this) + offset);
	}
public:
	// --------------------------- OFFSETS ---------------------------
	NETVAR("DT_BaseAttributableItem", "m_iItemDefinitionIndex", get_item_definition_index, item_definition_index);
	NETVAR("DT_BaseCombatWeapon", "m_flNextPrimaryAttack", get_next_primary_attack, float);
	NETVAR("DT_BaseCombatWeapon", "m_iClip1", get_ammo, int);
	NETVAR("DT_BaseCSGrenade", "m_bPinPulled", m_bPinPulled, bool);
	NETVAR("DT_BaseCSGrenade", "m_fThrowTime", m_fThrowTime, float);
	NETVAR("DT_BaseCombatWeapon", "m_iViewModelIndex", get_view_model_index, int);
	NETVAR("DT_BaseCombatWeapon", "m_iWorldModelIndex", get_world_model_index, int);
	NETVAR("DT_BaseCombatWeapon", "m_iWorldDroppedModelIndex", get_world_dropped_model_index, int);
	NETVAR("DT_BaseCombatWeapon", "m_hWeaponWorldModel", get_weapon_world_model, c_base_handle);
	NETVAR("DT_BaseAttributableItem", "m_iAccountID", get_account_id, int);
	NETVAR("DT_BaseAttributableItem", "m_iItemIDHigh", get_item_id_high, int);
	NETVAR("DT_BaseAttributableItem", "m_iEntityQuality", get_entity_quality, int);
	NETVAR("DT_BaseAttributableItem", "m_nFallbackPaintKit", get_fallback_paint_kit, unsigned);
	NETVAR("DT_BaseAttributableItem", "m_nFallbackSeed", get_fallback_seed, unsigned);
	NETVAR("DT_BaseAttributableItem", "m_flFallbackWear", get_fallback_wear, float);
	NETVAR("DT_BaseAttributableItem", "m_nFallbackStatTrak", get_fallback_stattrak, unsigned);
	
	// --------------------------- OTHERS ---------------------------
	weapon_info_t* get_cs_weapon_data() {
		if (!this) return nullptr;
		static auto system = *reinterpret_cast<c_weapon_system**>(global_utils::get_signature("client_panorama.dll", "8B 35 ? ? ? ? FF 10 0F B7 C0") + 0x2u);
		return system->get_weapon_data(this->get_item_definition_index());
	}
	std::string sanitized_weapon_name() {
		if (!this) return std::string();
		auto data = this->get_cs_weapon_data();
		if (!data) return std::string();
		std::string weapon_name = std::string(data->szWeaponName);
		weapon_name.erase(0, 7);
		return weapon_name;
	}

	float get_inaccuracy() {
		if (!(uintptr_t)this) return 0.0f;
		using original_fn = float(__thiscall*)(void*);
		return (*(original_fn**)this)[482](this);
	}

	float get_spread() {
		if (!(uintptr_t)this) return 0.0f;
		using original_fn = float(__thiscall*)(void*);
		return (*(original_fn**)this)[452](this);
	}

	void update_accuracy_penalty() {
		if (!(uintptr_t)this) return;
		using original_fn = void(__thiscall*)(void*);
		(*(original_fn**)this)[483](this);
	}

	int get_rage_config_id()
	{
		switch (get_item_definition_index())
		{
		case item_definition_index::WEAPON_GLOCK:
		case item_definition_index::WEAPON_HKP2000:
		case item_definition_index::WEAPON_USP_SILENCER:
		case item_definition_index::WEAPON_ELITE:
		case item_definition_index::WEAPON_P250:
		case item_definition_index::WEAPON_TEC9:
		case item_definition_index::WEAPON_FIVESEVEN:
		case item_definition_index::WEAPON_CZ75A: return 0;
		case item_definition_index::WEAPON_DEAGLE:
		case item_definition_index::WEAPON_REVOLVER: return 1;

		case item_definition_index::WEAPON_NOVA: 
		case item_definition_index::WEAPON_XM1014: 
		case item_definition_index::WEAPON_SAWEDOFF:
		case item_definition_index::WEAPON_MAG7: 
		case item_definition_index::WEAPON_M249: 
		case item_definition_index::WEAPON_NEGEV: 

		case item_definition_index::WEAPON_MAC10: 
		case item_definition_index::WEAPON_MP9: 
		case item_definition_index::WEAPON_MP7:
		case item_definition_index::WEAPON_MP5SD: 
		case item_definition_index::WEAPON_UMP45: 
		case item_definition_index::WEAPON_P90:
		case item_definition_index::WEAPON_BIZON: 

		case item_definition_index::WEAPON_GALILAR: 
		case item_definition_index::WEAPON_FAMAS: 
		case item_definition_index::WEAPON_AK47: 
		case item_definition_index::WEAPON_M4A1:
		case item_definition_index::WEAPON_SG556:
		case item_definition_index::WEAPON_AUG:
		case item_definition_index::WEAPON_M4A1_SILENCER: return 5;
		case item_definition_index::WEAPON_SSG08: return 2;
		case item_definition_index::WEAPON_AWP: return 3;
		case item_definition_index::WEAPON_G3SG1:
		case item_definition_index::WEAPON_SCAR20: return 4;
		default: return -1;
		}
	}

	int get_config_id()
	{
		switch (get_item_definition_index())
		{
		case item_definition_index::WEAPON_GLOCK: return 1;
		case item_definition_index::WEAPON_HKP2000: return 2;
		case item_definition_index::WEAPON_USP_SILENCER: return 3;
		case item_definition_index::WEAPON_ELITE: return 4;
		case item_definition_index::WEAPON_P250: return 5;
		case item_definition_index::WEAPON_TEC9: return 6;
		case item_definition_index::WEAPON_FIVESEVEN: return 7;
		case item_definition_index::WEAPON_CZ75A: return 8;
		case item_definition_index::WEAPON_DEAGLE: return 9;
		case item_definition_index::WEAPON_REVOLVER: return 10;

		case item_definition_index::WEAPON_NOVA: return 11;
		case item_definition_index::WEAPON_XM1014: return 12;
		case item_definition_index::WEAPON_SAWEDOFF: return 13;
		case item_definition_index::WEAPON_MAG7: return 14;
		case item_definition_index::WEAPON_M249: return 15;
		case item_definition_index::WEAPON_NEGEV: return 16;

		case item_definition_index::WEAPON_MAC10: return 17;
		case item_definition_index::WEAPON_MP9: return 18;
		case item_definition_index::WEAPON_MP7: return 19;
		case item_definition_index::WEAPON_MP5SD: return 20;
		case item_definition_index::WEAPON_UMP45: return 21;
		case item_definition_index::WEAPON_P90: return 22;
		case item_definition_index::WEAPON_BIZON: return 23;

		case item_definition_index::WEAPON_GALILAR: return 24;
		case item_definition_index::WEAPON_FAMAS: return 25;
		case item_definition_index::WEAPON_AK47: return 26;
		case item_definition_index::WEAPON_M4A1: return 27;
		case item_definition_index::WEAPON_M4A1_SILENCER: return 28;
		case item_definition_index::WEAPON_SSG08: return 29;
		case item_definition_index::WEAPON_SG556: return 30;
		case item_definition_index::WEAPON_AUG: return 31;
		case item_definition_index::WEAPON_AWP: return 32;
		case item_definition_index::WEAPON_G3SG1: return 33;
		case item_definition_index::WEAPON_SCAR20: return 34;

		case item_definition_index::WEAPON_KNIFE_BUTTERFLY: return 35;
		case item_definition_index::WEAPON_KNIFE_CANIS: return 36;
		case item_definition_index::WEAPON_KNIFE_CORD: return 37;
		case item_definition_index::WEAPON_KNIFE_CSS: return 38;
		case item_definition_index::WEAPON_KNIFE_FALCHION: return 39;
		case item_definition_index::WEAPON_KNIFE_FLIP: return 40;
		case item_definition_index::WEAPON_KNIFE_GUT: return 41;
		case item_definition_index::WEAPON_KNIFE_GYPSY_JACKKNIFE: return 42;
		case item_definition_index::WEAPON_KNIFE_KARAMBIT: return 43;
		case item_definition_index::WEAPON_KNIFE_M9_BAYONET: return 44;
		case item_definition_index::WEAPON_KNIFE_OUTDOOR: return 45;
		case item_definition_index::WEAPON_KNIFE_PUSH: return 46;
		case item_definition_index::WEAPON_KNIFE_SKELETON: return 47;
		case item_definition_index::WEAPON_KNIFE_STILETTO: return 48;
		case item_definition_index::WEAPON_KNIFE_SURVIVAL_BOWIE: return 49;
		case item_definition_index::WEAPON_KNIFE_TACTICAL: return 50;
		case item_definition_index::WEAPON_KNIFE_URSUS: return 51;
		case item_definition_index::WEAPON_KNIFE_WIDOWMAKER: return 52;
		case item_definition_index::WEAPON_BAYONET: return 53;

		default: return 0;
		}
	}
};

class c_base_view_model : public c_base_entity
{
public:
	NETVAR("DT_BaseViewModel", "m_hOwner", get_owner, c_base_handle);
	NETVAR("DT_BaseViewModel", "m_hWeapon", get_weapon, c_base_handle);
	NETVAR("DT_BaseViewModel", "m_nSequence", get_sequence_prop, recv_prop*);
};