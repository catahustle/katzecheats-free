#include "skinchanger.h"

#include "../../sdk/sdk.h"
#include "../../interfaces/interfaces.h"
#include "../../utilities/globals.h"
#include "../../menu/settings.h"
#include "../../sdk/weapon_definitions.h"

template <typename Key, typename Value>
struct Node_t
{
	int previous_id;		//0x0000
	int next_id;			//0x0004
	void* _unknown_ptr;		//0x0008
	int _unknown;			//0x000C
	Key key;				//0x0010
	Value value;			//0x0014
};

template <typename Key, typename Value>
struct Head_t
{
	Node_t<Key, Value>* memory;		//0x0000
	int allocation_count;			//0x0004
	int grow_size;					//0x0008
	int start_element;				//0x000C
	int next_available;				//0x0010
	int _unknown;					//0x0014
	int last_element;				//0x0018
}; //Size=0x001C

struct String_t
{
	char* buffer;	//0x0000
	int capacity;	//0x0004
	int grow_size;	//0x0008
	int length;		//0x000C
}; //Size=0x0010

struct CPaintKit
{
	int id;						//0x0000

	String_t name;				//0x0004
	String_t description;		//0x0014
	String_t item_name;			//0x0024
	String_t material_name;		//0x0034
	String_t image_inventory;	//0x0044

	char pad_0x0054[0x8C];		//0x0054
}; //Size=0x00E0

class CCStrike15ItemSchema;
class CCStrike15ItemSystem;

void features::skinchanger::initialize()
{
	const auto V_UCS2ToUTF8 = reinterpret_cast<int(*)(const wchar_t* ucs2, char* utf8, int len)>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "V_UCS2ToUTF8"));
	const auto sig_address = global_utils::get_signature("client_panorama.dll", "E8 ? ? ? ? FF 76 0C 8D 48 04 E8");
	const auto item_system_offset = *reinterpret_cast<std::int32_t*>(sig_address + 1);
	const auto item_system_fn = reinterpret_cast<CCStrike15ItemSystem * (*)()>(sig_address + 5 + item_system_offset);
	const auto item_schema = reinterpret_cast<CCStrike15ItemSchema*>(std::uintptr_t(item_system_fn()) + sizeof(void*));
	const auto get_paint_kit_definition_offset = *reinterpret_cast<std::int32_t*>(sig_address + 11 + 1);
	const auto get_paint_kit_definition_fn = reinterpret_cast<CPaintKit * (__thiscall*)(CCStrike15ItemSchema*, int)>(sig_address + 11 + 5 + get_paint_kit_definition_offset);
	const auto start_element_offset = *reinterpret_cast<std::intptr_t*>(std::uintptr_t(get_paint_kit_definition_fn) + 8 + 2);
	const auto head_offset = start_element_offset - 12;
	const auto map_head = reinterpret_cast<Head_t<int, CPaintKit*>*>(std::uintptr_t(item_schema) + head_offset);

	for (auto i = 0; i <= map_head->last_element; ++i)
	{
		const auto paint_kit = map_head->memory[i].value;

		if (paint_kit->id == 9001)
			continue;

		const wchar_t* wide_name = I::localize->find(paint_kit->item_name.buffer + 1);

		char name[256];
		V_UCS2ToUTF8(wide_name, name, sizeof(name));

		if (paint_kit->id < 10000)
			this->skin_kits.push_back({ paint_kit->id, name });
		else
			this->glove_kits.push_back({ paint_kit->id, name });
	}

	std::sort(this->skin_kits.begin(), this->skin_kits.end());
	std::sort(this->glove_kits.begin(), this->glove_kits.end());
}

struct weapon_info
{
	constexpr weapon_info(const char* model, const char* icon = nullptr) :
		model(model),
		icon(icon)
	{}

	const char* model;
	const char* icon;
};

weapon_info* get_weapon_info(item_definition_index defindex)
{
	static std::map<item_definition_index, weapon_info> info =
	{
		{item_definition_index::WEAPON_KNIFE,{"models/weapons/v_knife_default_ct.mdl", "knife_default_ct"}},
		{item_definition_index::WEAPON_KNIFE_T,{"models/weapons/v_knife_default_t.mdl", "knife_t"}},
		{item_definition_index::WEAPON_BAYONET, {"models/weapons/v_knife_bayonet.mdl", "bayonet"}},
		{item_definition_index::WEAPON_KNIFE_CSS, {"models/weapons/v_knife_css.mdl", "knife_css"}},
		{item_definition_index::WEAPON_KNIFE_FLIP, {"models/weapons/v_knife_flip.mdl", "knife_flip"}},
		{item_definition_index::WEAPON_KNIFE_GUT, {"models/weapons/v_knife_gut.mdl", "knife_gut"}},
		{item_definition_index::WEAPON_KNIFE_KARAMBIT, {"models/weapons/v_knife_karam.mdl", "knife_karambit"}},
		{item_definition_index::WEAPON_KNIFE_M9_BAYONET, {"models/weapons/v_knife_m9_bay.mdl", "knife_m9_bayonet"}},
		{item_definition_index::WEAPON_KNIFE_TACTICAL, {"models/weapons/v_knife_tactical.mdl", "knife_tactical"}},
		{item_definition_index::WEAPON_KNIFE_FALCHION, {"models/weapons/v_knife_falchion_advanced.mdl", "knife_falchion"}},
		{item_definition_index::WEAPON_KNIFE_SURVIVAL_BOWIE, {"models/weapons/v_knife_survival_bowie.mdl", "knife_survival_bowie"}},
		{item_definition_index::WEAPON_KNIFE_BUTTERFLY, {"models/weapons/v_knife_butterfly.mdl", "knife_butterfly"}},
		{item_definition_index::WEAPON_KNIFE_PUSH, {"models/weapons/v_knife_push.mdl", "knife_push"}},
		{item_definition_index::WEAPON_KNIFE_CORD, {"models/weapons/v_knife_cord.mdl", "knife_cord"}},
		{item_definition_index::WEAPON_KNIFE_CANIS, {"models/weapons/v_knife_canis.mdl", "knife_canis"}},
		{item_definition_index::WEAPON_KNIFE_URSUS,{"models/weapons/v_knife_ursus.mdl", "knife_ursus"}},
		{item_definition_index::WEAPON_KNIFE_GYPSY_JACKKNIFE,{"models/weapons/v_knife_gypsy_jackknife.mdl", "knife_gypsy_jackknife"}},
		{item_definition_index::WEAPON_KNIFE_OUTDOOR,{"models/weapons/v_knife_outdoor.mdl", "knife_outdoor"}},
		{item_definition_index::WEAPON_KNIFE_STILETTO,{"models/weapons/v_knife_stiletto.mdl", "knife_stiletto"}},
		{item_definition_index::WEAPON_KNIFE_WIDOWMAKER,{"models/weapons/v_knife_widowmaker.mdl", "knife_widowmaker"}},
		{item_definition_index::WEAPON_KNIFE_SKELETON,{"models/weapons/v_knife_skeleton.mdl", "knife_skeleton"}},
		{item_definition_index::GLOVE_STUDDED_BLOODHOUND,{"models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"}},
		{item_definition_index::GLOVE_T_SIDE,{"models/weapons/v_models/arms/glove_fingerless/v_glove_fingerless.mdl"}},
		{item_definition_index::GLOVE_CT_SIDE,{"models/weapons/v_models/arms/glove_hardknuckle/v_glove_hardknuckle.mdl"}},
		{item_definition_index::GLOVE_SPORTY,{"models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"}},
		{item_definition_index::GLOVE_SLICK,{"models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"}},
		{item_definition_index::GLOVE_LEATHER_WRAP,{"models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"}},
		{item_definition_index::GLOVE_MOTORCYCLE,{"models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"}},
		{item_definition_index::GLOVE_SPECIALIST,{"models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"}},
		{item_definition_index::GLOVE_HYDRA,{"models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound_hydra.mdl"}}
	};

	const auto entry = info.find(defindex);
	return entry == end(info) ? nullptr : &entry->second;
}

item_definition_index get_knife_idx()
{
	switch (c_config::skins::knife_id)
	{
	case 35 - 34: return item_definition_index::WEAPON_KNIFE_BUTTERFLY;
	case 36 - 34: return item_definition_index::WEAPON_KNIFE_CANIS;
	case 37 - 34: return item_definition_index::WEAPON_KNIFE_CORD;
	case 38 - 34: return item_definition_index::WEAPON_KNIFE_CSS;
	case 39 - 34: return item_definition_index::WEAPON_KNIFE_FALCHION;
	case 40 - 34: return item_definition_index::WEAPON_KNIFE_FLIP;
	case 41 - 34: return item_definition_index::WEAPON_KNIFE_GUT;
	case 42 - 34: return item_definition_index::WEAPON_KNIFE_GYPSY_JACKKNIFE;
	case 43 - 34: return item_definition_index::WEAPON_KNIFE_KARAMBIT;
	case 44 - 34: return item_definition_index::WEAPON_KNIFE_M9_BAYONET;
	case 45 - 34: return item_definition_index::WEAPON_KNIFE_OUTDOOR;
	case 46 - 34: return item_definition_index::WEAPON_KNIFE_PUSH;
	case 47 - 34: return item_definition_index::WEAPON_KNIFE_SKELETON;
	case 48 - 34: return item_definition_index::WEAPON_KNIFE_STILETTO;
	case 49 - 34: return item_definition_index::WEAPON_KNIFE_SURVIVAL_BOWIE;
	case 50 - 34: return item_definition_index::WEAPON_KNIFE_TACTICAL;
	case 51 - 34: return item_definition_index::WEAPON_KNIFE_URSUS;
	case 52 - 34: return item_definition_index::WEAPON_KNIFE_WIDOWMAKER;
	case 53 - 34: return item_definition_index::WEAPON_BAYONET;
	}
}

void features::skinchanger::apply()
{
	if (!I::engine_client->in_game_connected()) return;
	c_base_entity* local = I::client_ent_list->get_client_entity(I::engine_client->get_local_player());
	if (!local) return;
	if (!local->is_alive()) return;

	auto weapons = local->get_weapons();
	for (auto handle : weapons)
	{
		auto weapon = (c_base_combat_weapon*)I::client_ent_list->get_client_entity_from_handle(handle);
		if (!weapon)
			return;

		auto id = weapon->get_config_id();

		auto wpn_data = weapon->get_cs_weapon_data();
		if (!wpn_data) continue;

		if (weapon->get_item_definition_index() == item_definition_index::WEAPON_C4 ||
			weapon->get_item_definition_index() == item_definition_index::WEAPON_TASER || 
			wpn_data->iWeaponType == cs_weapon_type::WEAPONTYPE_GRENADE)
			continue;

		bool is_knife = wpn_data->iWeaponType == cs_weapon_type::WEAPONTYPE_KNIFE;
		if (is_knife && !c_config::skins::knife_id)
			continue;

		int cfg_idx = !is_knife ? c_config::skins::skins[id].id : c_config::skins::knife.id;

		if (cfg_idx)
		{
			int paint_kit = this->skin_kits[cfg_idx].id;

			weapon->get_item_id_high() = -1;
			weapon->get_fallback_paint_kit() = paint_kit;
			weapon->get_fallback_wear() = !is_knife ? c_config::skins::skins[id].wear : c_config::skins::knife.wear;
		}

		if (is_knife)
		{
			weapon->get_item_definition_index() = get_knife_idx();
			weapon->set_model_index(I::model_info->get_model_index(get_weapon_info(get_knife_idx())->model));
			weapon->GetClientNetworkable()->PreDataUpdate(0);

			auto view_model = (c_base_view_model*)I::client_ent_list->get_client_entity_from_handle(local->get_view_model());
			if (!view_model)
				continue;

			auto view_model_weapon = (c_base_combat_weapon*)I::client_ent_list->get_client_entity_from_handle(view_model->get_weapon());
			if (!view_model_weapon)
				continue;

			auto info = get_weapon_info(view_model_weapon->get_item_definition_index());
			if (!info)
				continue;

			auto mdl_idx = I::model_info->get_model_index(info->model);
			view_model->get_model_index() = mdl_idx;

			const auto world_model = (c_base_entity*)I::client_ent_list->get_client_entity_from_handle(view_model_weapon->get_weapon_world_model());

			if (!world_model)
				continue;

			world_model->get_model_index() = mdl_idx + 1;
		}
	}
}

__forceinline int random_sequence(const int low, const int high)
{
	return rand() % (high - low + 1) + low;
}

int new_anim(fnv_hash model, int sequence)
{
	enum ESequence
	{
		SEQUENCE_DEFAULT_DRAW = 0,
		SEQUENCE_DEFAULT_IDLE1 = 1,
		SEQUENCE_DEFAULT_IDLE2 = 2,
		SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
		SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
		SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
		SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
		SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
		SEQUENCE_DEFAULT_LOOKAT01 = 12,

		SEQUENCE_BUTTERFLY_DRAW = 0,
		SEQUENCE_BUTTERFLY_DRAW2 = 1,
		SEQUENCE_BUTTERFLY_LOOKAT01 = 13,
		SEQUENCE_BUTTERFLY_LOOKAT03 = 15,

		SEQUENCE_FALCHION_IDLE1 = 1,
		SEQUENCE_FALCHION_HEAVY_MISS1 = 8,
		SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP = 9,
		SEQUENCE_FALCHION_LOOKAT01 = 12,
		SEQUENCE_FALCHION_LOOKAT02 = 13,

		SEQUENCE_CSS_LOOKAT01 = 14,
		SEQUENCE_CSS_LOOKAT02 = 15,

		SEQUENCE_DAGGERS_IDLE1 = 1,
		SEQUENCE_DAGGERS_LIGHT_MISS1 = 2,
		SEQUENCE_DAGGERS_LIGHT_MISS5 = 6,
		SEQUENCE_DAGGERS_HEAVY_MISS2 = 11,
		SEQUENCE_DAGGERS_HEAVY_MISS1 = 12,

		SEQUENCE_BOWIE_IDLE1 = 1,
	};

	switch (model)
	{
	case fnv_hash("models/weapons/v_knife_butterfly.mdl"):
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
			return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
		default:
			return sequence + 1;
		}
	}
	case fnv_hash("models/weapons/v_knife_falchion_advanced.mdl"):
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_FALCHION_IDLE1;
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return random_sequence(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence - 1;
		}
	}
	case fnv_hash("models/weapons/v_knife_css.mdl"):
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence(SEQUENCE_CSS_LOOKAT01, SEQUENCE_CSS_LOOKAT02);
		default:
			return sequence;
		}
	}
	case fnv_hash("models/weapons/v_knife_push.mdl"):
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_DAGGERS_IDLE1;
		case SEQUENCE_DEFAULT_LIGHT_MISS1:
		case SEQUENCE_DEFAULT_LIGHT_MISS2:
			return random_sequence(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return random_sequence(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
		case SEQUENCE_DEFAULT_HEAVY_HIT1:
		case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
		case SEQUENCE_DEFAULT_LOOKAT01:
			return sequence + 3;
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence + 2;
		}
	}
	case fnv_hash("models/weapons/v_knife_survival_bowie.mdl"):
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_BOWIE_IDLE1;
		default:
			return sequence - 1;
		}
	}
	case fnv_hash("models/weapons/v_knife_ursus.mdl"):
	case fnv_hash("models/weapons/v_knife_cord.mdl"):
	case fnv_hash("models/weapons/v_knife_canis.mdl"):
	case fnv_hash("models/weapons/v_knife_outdoor.mdl"):
	case fnv_hash("models/weapons/v_knife_skeleton.mdl"):
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
			return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
		default:
			return sequence + 1;
		}
	}
	case fnv_hash("models/weapons/v_knife_stiletto.mdl"):
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence(12, 13);
		default:
			return sequence;
		}
	}
	case fnv_hash("models/weapons/v_knife_widowmaker.mdl"):
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence(14, 15);
		default:
			return sequence;
		}
	}
	default:
		return sequence;
	}
}

void features::skinchanger::fix_sequence(c_recv_proxy_data* data, c_base_view_model* view_model)
{
	if (!I::engine_client->in_game_connected()) return;
	auto local = I::client_ent_list->get_client_entity(I::engine_client->get_local_player());
	if (!local) return;
	if (!local->is_alive()) return;
	
	auto weapon = local->active_weapon();
	if (!weapon) return;
	auto wpn_data = weapon->get_cs_weapon_data();
	if (!wpn_data) return;
	if (wpn_data->iWeaponType != cs_weapon_type::WEAPONTYPE_KNIFE) return;

	auto owner = I::client_ent_list->get_client_entity_from_handle(view_model->get_owner());
	if (owner != local) 
		return;

	auto view_model_weapon = (c_base_combat_weapon*)I::client_ent_list->get_client_entity_from_handle(view_model->get_weapon());
	if (!view_model_weapon)
		return;

	auto info = get_weapon_info(view_model_weapon->get_item_definition_index());
	if (!info)
		return;

	auto& sequence = data->value.m_int;
	sequence = new_anim(fnv_hash(info->model), sequence);
}