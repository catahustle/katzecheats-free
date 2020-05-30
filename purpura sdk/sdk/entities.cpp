#include "entities.h"
#include "i_base_handle.h"
#include "../interfaces/interfaces.h"

bool c_base_entity::is_visible()
{
	if (!(uintptr_t)this) return false;
	c_base_entity* local = interfaces::client_ent_list->get_client_entity(interfaces::engine_client->get_local_player());
	if (!local) return false;
	c_ray ray(local->origin() + local->view_offset(), this->origin() + this->view_offset());
	c_trace_filter filter(local);
	c_trace tr;

	interfaces::engine_trace->trace_ray(ray, mask_shot | contents_grate, &filter, &tr);

	return tr.entity == this || tr.flFraction > 0.97f;
}

c_base_combat_weapon* c_base_entity::active_weapon() {
	if (!(uintptr_t)this) return nullptr;
	static int offset = netvar_manager->get_offset("DT_BaseCombatCharacter", "m_hActiveWeapon");
	const auto weapon_data = get_value<c_base_handle>(offset);
	return reinterpret_cast<c_base_combat_weapon*>(interfaces::client_ent_list->get_client_entity_from_handle(weapon_data));
}

void c_base_entity::invalidate_bone_cache()
{
	static DWORD addr = (DWORD)global_utils::get_signature("client_panorama.dll", "80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81");

	*(int*)((uintptr_t)this + 0xA30) = interfaces::global_vars->framecount; //we'll skip occlusion checks now
	*(int*)((uintptr_t)this + 0xA28) = 0;//clear occlusion flags

	unsigned long g_iModelBoneCounter = **(unsigned long**)(addr + 10);
	*(unsigned int*)((DWORD)this + 0x2924) = 0xFF7FFFFF; // m_flLastBoneSetupTime = -FLT_MAX;
	*(unsigned int*)((DWORD)this + 0x2690) = (g_iModelBoneCounter - 1); // m_iMostRecentModelBoneCounter = g_iModelBoneCounter - 1;
}

int c_base_entity::get_sequence_activity(int seq)
{
	auto hdr = interfaces::model_info->get_studio_model(model());
	if (!hdr)
		return -1;

	static auto GetSequenceActivity = reinterpret_cast<int(__fastcall*)(void*, studio_hdr_t*, int)>(global_utils::get_signature("client_panorama.dll", "55 8B EC 53 8B 5D 08 56 8B F1 83"));
	if (!GetSequenceActivity)
		return -1;

	return GetSequenceActivity(this, hdr, seq);
}