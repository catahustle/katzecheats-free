#define _USE_MATH_DEFINES
#include <cmath>

#include "autowall.h"
#include "../legit/legit_lag_comp.h"

#include "../../sdk/sdk.h"
#include "../../interfaces/interfaces.h"
#include "../../utilities/globals.h"
#include "../../utilities/math.h"
#include "../../menu/settings.h"

#define DMG_BULLET			(1 << 1)	// shot
#define DMG_SLASH			(1 << 2)	// cut, clawed, stabbed
#define DMG_BURN			(1 << 3)	// heat burned
#define DMG_VEHICLE			(1 << 4)	// hit by a vehicle
#define DMG_FALL			(1 << 5)	// fell too far
#define DMG_BLAST			(1 << 6)	// explosive blast damage
#define DMG_CLUB			(1 << 7)	// crowbar, punch, headbutt
#define DMG_SHOCK			(1 << 8)	// electric shock
#define DMG_SONIC			(1 << 9)	// sound pulse shockwave
#define DMG_ENERGYBEAM		(1 << 10)	// laser or other high energy beam 
#define DMG_PREVENT_PHYSICS_FORCE		(1 << 11)	// Prevent a physics force 
#define DMG_NEVERGIB		(1 << 12)	// with this bit OR'd in, no damage type will be able to gib victims upon death
#define DMG_ALWAYSGIB		(1 << 13)	// with this bit OR'd in, any damage type can be made to gib victims upon death.
#define DMG_DROWN			(1 << 14)	// Drowning

#ifndef MIN
#define MIN( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#endif

#ifndef MAX
#define MAX( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#endif

inline vec3_t operator*(float fl, const vec3_t& v)
{
	return v * fl;
}

void GetBulletTypeParameters(float& maxRange, float& maxDistance, char* bulletType, bool sv_penetration_type)
{
	if (sv_penetration_type)
	{
		maxRange = 35.0;
		maxDistance = 3000.0;
	}
	else
	{
		//Play tribune to framerate. Thanks, stringcompare
		//Regardless I doubt anyone will use the old penetration system anyway; so it won't matter much.
		if (!strcmp(bulletType, "BULLET_PLAYER_338MAG"))
		{
			maxRange = 45.0;
			maxDistance = 8000.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_762MM"))
		{
			maxRange = 39.0;
			maxDistance = 5000.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_556MM") || !strcmp(bulletType, "BULLET_PLAYER_556MM_SMALL") || !strcmp(bulletType, "BULLET_PLAYER_556MM_BOX"))
		{
			maxRange = 35.0;
			maxDistance = 4000.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_57MM"))
		{
			maxRange = 30.0;
			maxDistance = 2000.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_50AE"))
		{
			maxRange = 30.0;
			maxDistance = 1000.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_357SIG") || !strcmp(bulletType, "BULLET_PLAYER_357SIG_SMALL") || !strcmp(bulletType, "BULLET_PLAYER_357SIG_P250") || !strcmp(bulletType, "BULLET_PLAYER_357SIG_MIN"))
		{
			maxRange = 25.0;
			maxDistance = 800.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_9MM"))
		{
			maxRange = 21.0;
			maxDistance = 800.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_45ACP"))
		{
			maxRange = 15.0;
			maxDistance = 500.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_BUCKSHOT"))
		{
			maxRange = 0.0;
			maxDistance = 0.0;
		}
	}
}

inline void UTIL_TraceLineIgnoreTwoEntities(const vec3_t& vecAbsStart, const vec3_t& vecAbsEnd, unsigned int mask,
	const c_base_entity* ignore, const c_base_entity* ignore2, int collisionGroup, c_trace* ptr)
{
	c_ray ray(vecAbsStart, vecAbsEnd);
	c_trace_filter_skip_two_entities traceFilter((void*)ignore, (void*)ignore2);
	I::engine_trace->trace_ray(ray, mask, &traceFilter, ptr);
}

FORCEINLINE vec_t DotProduct(const vec3_t& a, const vec3_t& b)
{
	return(a.x * b.x + a.y * b.y + a.z * b.z);
}

inline float DistanceToRay(const vec3_t& pos, const vec3_t& rayStart, const vec3_t& rayEnd, float* along = NULL, vec3_t* pointOnRay = NULL)
{
	vec3_t to = pos - rayStart;
	vec3_t dir = rayEnd - rayStart;
	float length = dir.NormalizeInPlace();

	float rangeAlong = DotProduct(dir, to);
	if (along)
	{
		*along = rangeAlong;
	}

	float range;

	if (rangeAlong < 0.0f)
	{
		// off start point
		range = -(pos - rayStart).Length();

		if (pointOnRay)
		{
			*pointOnRay = rayStart;
		}
	}
	else if (rangeAlong > length)
	{
		// off end point
		range = -(pos - rayEnd).Length();

		if (pointOnRay)
		{
			*pointOnRay = rayEnd;
		}
	}
	else // within ray bounds
	{
		vec3_t temp1 = dir;
		temp1 *= rangeAlong;
		vec3_t onRay = rayStart + temp1;
		range = (pos - onRay).Length();

		if (pointOnRay)
		{
			*pointOnRay = onRay;
		}
	}

	return range;
}

void UTIL_ClipTraceToPlayers(const vec3_t& vecAbsStart, const vec3_t& vecAbsEnd, unsigned int mask, i_trace_filter* filter, c_trace* tr)
{
	c_trace playerTrace;
	c_ray ray(vecAbsStart, vecAbsEnd);
	float smallestFraction = tr->flFraction;
	const float maxRange = 60.0f;

	for (int k = 1; k <= I::global_vars->max_clients; ++k)
	{
		c_base_entity* player = I::client_ent_list->get_client_entity(k);

		if (!player)
			continue;
		if (!player->is_alive())
			continue;

#ifdef CLIENT_DLL
		if (player->IsDormant())
			continue;
#endif // CLIENT_DLL

		if (filter && filter->should_hit_entity(player, mask) == false)
			continue;

		float range = DistanceToRay(player->abs_origin(), vecAbsStart, vecAbsEnd);
		if (range < 0.0f || range > maxRange)
			continue;

		I::engine_trace->clip_ray_to_entity(ray, mask | contents_hitbox, player, &playerTrace);
		if (playerTrace.flFraction < smallestFraction)
		{
			// we shortened the ray - save off the trace
			*tr = playerTrace;
			smallestFraction = playerTrace.flFraction;
		}
	}
}

void scale_damage(c_base_entity* ent, c_trace& enterTrace, weapon_info_t* weaponData, float& currentDamage, bool local = false)
{
	//Cred. to N0xius for reversing this.
	//TODO: _xAE^; look into reversing this yourself sometime

	bool hasHeavyArmor = ent->heavy_armour();
	int armorValue = ent->armour();
	int hitGroup = local ? 1 : enterTrace.hitGroup;

	//Fuck making a new function, lambda beste. ~ Does the person have armor on for the hitbox checked?
	auto IsArmored = [&]()->bool
	{
		switch (hitGroup)
		{
		case hitgroup_head:
			return !!ent->helmet(); //Fuck compiler errors - force-convert it to a bool via (!!)
		case hitgroup_generic:
		case hitgroup_chest:
		case hitgroup_stomach:
		case hitgroup_leftarm:
		case hitgroup_rightarm:
			return true;
		default:
			return false;
		}
	};

	switch (hitGroup)
	{
	case hitgroup_head:
		currentDamage *= hasHeavyArmor ? 2.f : 4.f; //Heavy Armor does 1/2 damage
		break;
	case hitgroup_stomach:
		currentDamage *= 1.25f;
		break;
	case hitgroup_leftleg:
	case hitgroup_rightleg:
		currentDamage *= 0.75f;
		break;
	default:
		break;
	}

	if (armorValue > 0 && IsArmored())
	{
		float bonusValue = 1.f, armorBonusRatio = 0.5f, armorRatio = weaponData->flArmorRatio / 2.f;

		//Damage gets modified for heavy armor users
		if (hasHeavyArmor)
		{
			armorBonusRatio = 0.33f;
			armorRatio *= 0.5f;
			bonusValue = 0.33f;
		}

		auto NewDamage = currentDamage * armorRatio;

		if (hasHeavyArmor)
			NewDamage *= 0.85f;

		if (((currentDamage - (currentDamage * armorRatio)) * (bonusValue * armorBonusRatio)) > armorValue)
			NewDamage = currentDamage - (armorValue / armorBonusRatio);

		currentDamage = NewDamage;
	}
}

inline int UTIL_PointContents(const vec3_t& vec, int contentsMask)
{
	return I::engine_trace->get_point_contents(vec, contentsMask);
}

inline void UTIL_TraceLine(const vec3_t& vecAbsStart, const vec3_t& vecAbsEnd, unsigned int mask,
	i_trace_filter* pFilter, c_trace* ptr)
{
	c_ray ray(vecAbsStart, vecAbsEnd);
	I::engine_trace->trace_ray(ray, mask, pFilter, ptr);
}

inline void UTIL_TraceLine(const vec3_t& vecAbsStart, const vec3_t& vecAbsEnd, unsigned int mask,
	c_base_entity* ignore, int collisionGroup, c_trace* ptr)
{
	c_ray ray(vecAbsStart, vecAbsEnd);
	c_trace_filter traceFilter(ignore);

	I::engine_trace->trace_ray(ray, mask, &traceFilter, ptr);
}

bool IsBreakableEntity(c_base_entity* entity)
{
	if (!entity || entity->EntIndex() == 0)
		return false;

	static auto is_breakable_entity = (bool(__thiscall*)(c_base_entity*))global_utils::get_signature("client_panorama.dll", "55 8B EC 51 56 8B F1 85 F6 74 68 83");
	return is_breakable_entity(entity);
}

static bool TraceToExit(vec3_t start, vec3_t dir, vec3_t& end, c_trace& trEnter, c_trace& trExit, float flStepSize, float flMaxDistance)
{
	float flDistance = 0;
	vec3_t last = start;
	int nStartContents = 0;

	while (flDistance <= flMaxDistance)
	{
		flDistance += flStepSize;

		end = start + (flDistance * dir);

		vec3_t vecTrEnd = end - (flStepSize * dir);

		if (nStartContents == 0)
			nStartContents = UTIL_PointContents(end, cs_mask_shoot | contents_hitbox);

		int nCurrentContents = UTIL_PointContents(end, cs_mask_shoot | contents_hitbox);

		if ((nCurrentContents & cs_mask_shoot) == 0 || ((nCurrentContents & contents_hitbox) && nStartContents != nCurrentContents))
		{
			// this gets a bit more complicated and expensive when we have to deal with displacements
			UTIL_TraceLine(end, vecTrEnd, cs_mask_shoot | contents_hitbox, NULL, &trExit);

			// we exited the wall into a player's hitbox
			if (trExit.startSolid == true && (trExit.surface.flags & 0x8000)/*( nStartContents & CONTENTS_HITBOX ) == 0 && (nCurrentContents & CONTENTS_HITBOX)*/)
			{
				// do another trace, but skip the player to get the actual exit surface 
				UTIL_TraceLine(end, start, cs_mask_shoot, trExit.entity, COLLISION_GROUP_NONE, &trExit);
				if (trExit.did_hit() && trExit.startSolid == false)
				{
					end = trExit.end;
					return true;
				}
			}
			else if (trExit.did_hit() && trExit.startSolid == false)
			{
				bool bStartIsNodraw = !!(trEnter.surface.flags & (0x0080));
				bool bExitIsNodraw = !!(trExit.surface.flags & (0x0080));
				if (bExitIsNodraw && IsBreakableEntity(trExit.entity) && IsBreakableEntity(trEnter.entity))
				{
					// we have a case where we have a breakable object, but the mapper put a nodraw on the backside
					end = trExit.end;
					return true;
				}
				else if (bExitIsNodraw == false || (bStartIsNodraw && bExitIsNodraw)) // exit nodraw is only valid if our entrace is also nodraw
				{
					vec3_t vecNormal = trExit.plane.normal;
					float flDot = dir.Dot(vecNormal);
					if (flDot <= 1.0f)
					{
						// get the real end pos
						end = end - ((flStepSize * trExit.flFraction) * dir);
						return true;
					}
				}
			}
			else if (trEnter.did_hit_non_world_ent() && IsBreakableEntity(trEnter.entity))
			{
				// if we hit a breakable, make the assumption that we broke it if we can't find an exit (hopefully..)
				// fake the end pos
				trExit = trEnter;
				trExit.end = start + (1.0f * dir);
				return true;
			}
		}
	}

	return false;
}

bool HandleBulletPenetration(float& flPenetration,
	int& iEnterMaterial,
	bool& hitGrate,
	c_trace& tr,
	vec3_t& vecDir,
	surfacedata_t* pSurfaceData,
	float flPenetrationModifier,
	float flDamageModifier,
	bool bDoEffects,
	int iDamageType,
	float flPenetrationPower,
	int& nPenetrationCount,
	vec3_t& vecSrc,
	float flDistance,
	float flCurrentDistance,
	float& fCurrentDamage)
{
	bool bIsNodraw = !!(tr.surface.flags & (0x80));

	bool bFailedPenetrate = false;

	// check if bullet can penetrarte another entity
	if (nPenetrationCount == 0 && !hitGrate && !bIsNodraw
		&& iEnterMaterial != CHAR_TEX_GLASS && iEnterMaterial != CHAR_TEX_GRATE)
		bFailedPenetrate = true; // no, stop

	// If we hit a grate with iPenetration == 0, stop on the next thing we hit
	if (flPenetration <= 0 || nPenetrationCount <= 0)
		bFailedPenetrate = true;

	vec3_t penetrationEnd;

	c_trace exitTr;
	if (!TraceToExit(tr.end, vecDir, penetrationEnd, tr, exitTr, 4, 90))
	{
		// ended in solid
		if ((UTIL_PointContents(tr.end, cs_mask_shoot) & cs_mask_shoot) == 0)
		{
			bFailedPenetrate = true;
		}
	}

	if (bFailedPenetrate == true)
		return true;

	surfacedata_t* pExitSurfaceData = I::phys_surface->surface_data(exitTr.surface.surface_props);
	int iExitMaterial = pExitSurfaceData->game.material;
	if (I::game_cvar->find_var("sv_penetration_type")->get_int() == 1)
	{
		// percent of total damage lost automatically on impacting a surface
		float flDamLostPercent = 0.16;

		// since some railings in de_inferno are CONTENTS_GRATE but CHAR_TEX_CONCRETE, we'll trust the
		// CONTENTS_GRATE and use a high damage modifier.
		if (hitGrate || bIsNodraw || iEnterMaterial == CHAR_TEX_GLASS || iEnterMaterial == CHAR_TEX_GRATE)
		{
			// If we're a concrete grate (TOOLS/TOOLSINVISIBLE texture) allow more penetrating power.
			if (iEnterMaterial == CHAR_TEX_GLASS || iEnterMaterial == CHAR_TEX_GRATE)
			{
				flPenetrationModifier = 3.0f;
				flDamLostPercent = 0.05;
			}
			else
				flPenetrationModifier = 1.0f;

			flDamageModifier = 0.99f;
		}
		else if (iEnterMaterial == CHAR_TEX_FLESH && I::game_cvar->find_var("ff_damage_reduction_bullets")->get_float() == 0
			&& tr.entity && tr.entity->get_client_class()->class_id == ClassId_CCSPlayer && tr.entity->team() == G::local_player->team())
		{
			if (I::game_cvar->find_var("ff_damage_bullet_penetration")->get_float() == 0)
			{
				// don't allow penetrating players when FF is off
				flPenetrationModifier = 0;
				return true;
			}

			flPenetrationModifier = I::game_cvar->find_var("ff_damage_bullet_penetration")->get_float();
			flDamageModifier = I::game_cvar->find_var("ff_damage_bullet_penetration")->get_float();
		}
		else
		{
			// check the exit material and average the exit and entrace values
			float flExitPenetrationModifier = pExitSurfaceData->game.fl_penetration_modifier;
			float flExitDamageModifier = pExitSurfaceData->game.fl_damage_modifier;
			flPenetrationModifier = (flPenetrationModifier + flExitPenetrationModifier) / 2;
			flDamageModifier = (flDamageModifier + flExitDamageModifier) / 2;
		}

		// if enter & exit point is wood we assume this is 
		// a hollow crate and give a penetration bonus
		if (iEnterMaterial == iExitMaterial)
		{
			if (iExitMaterial == CHAR_TEX_WOOD || iExitMaterial == CHAR_TEX_CARDBOARD)
			{
				flPenetrationModifier = 3;
			}
			else if (iExitMaterial == CHAR_TEX_PLASTIC)
			{
				flPenetrationModifier = 2;
			}
		}

		float flTraceDistance = VectorLength(exitTr.end - tr.end);

		float flPenMod = MAX(0, (1 / flPenetrationModifier));

		float flPercentDamageChunk = fCurrentDamage * flDamLostPercent;
		float flPenWepMod = flPercentDamageChunk + MAX(0, (3 / flPenetrationPower) * 1.25) * (flPenMod * 3.0);

		float flLostDamageObject = ((flPenMod * (flTraceDistance * flTraceDistance)) / 24);
		float flTotalLostDamage = flPenWepMod + flLostDamageObject;

		// reduce damage power each time we hit something other than a grate
		fCurrentDamage -= MAX(0, flTotalLostDamage);

		if (fCurrentDamage < 1)
			return true;

		//flPenetrationPower -= (flTraceDistance/2) / flPenMod;
		flCurrentDistance += flTraceDistance;

		// NDebugOverlay::Box( exitTr.endpos, Vector(-2,-2,-2), Vector(2,2,2), 0,255,0,127, 8 );

		vecSrc = exitTr.end;
		flDistance = (flDistance - flCurrentDistance) * 0.5;

		nPenetrationCount--;
		return false;
	}
	else
	{
		// since some railings in de_inferno are CONTENTS_GRATE but CHAR_TEX_CONCRETE, we'll trust the
		// CONTENTS_GRATE and use a high damage modifier.
		if (hitGrate || bIsNodraw)
		{
			// If we're a concrete grate (TOOLS/TOOLSINVISIBLE texture) allow more penetrating power.
			flPenetrationModifier = 1.0f;
			flDamageModifier = 0.99f;
		}
		else
		{
			// Check the exit material to see if it is has less penetration than the entrance material.
			float flExitPenetrationModifier = pExitSurfaceData->game.fl_penetration_modifier;
			float flExitDamageModifier = pExitSurfaceData->game.fl_damage_modifier;
			if (flExitPenetrationModifier < flPenetrationModifier)
			{
				flPenetrationModifier = flExitPenetrationModifier;
			}
			if (flExitDamageModifier < flDamageModifier)
			{
				flDamageModifier = flExitDamageModifier;
			}
		}

		// if enter & exit point is wood or metal we assume this is 
		// a hollow crate or barrel and give a penetration bonus
		if (iEnterMaterial == iExitMaterial)
		{
			if (iExitMaterial == CHAR_TEX_WOOD ||
				iExitMaterial == CHAR_TEX_METAL)
			{
				flPenetrationModifier *= 2;
			}
		}

		float flTraceDistance = VectorLength(exitTr.end - tr.end);

		// check if bullet has enough power to penetrate this distance for this material
		if (flTraceDistance > (flPenetrationPower * flPenetrationModifier))
			return true; // bullet hasn't enough power to penetrate this distance

		// reduce damage power each time we hit something other than a grate
		fCurrentDamage *= flDamageModifier;

		//setup new start end parameters for successive trace

		flPenetrationPower -= flTraceDistance / flPenetrationModifier;
		flCurrentDistance += flTraceDistance;

		// NDebugOverlay::Box( exitTr.endpos, Vector(-2,-2,-2), Vector(2,2,2), 0,255,0,127, 8 );

		vecSrc = exitTr.end;
		flDistance = (flDistance - flCurrentDistance) * 0.5;

		// reduce penetration counter
		nPenetrationCount--;
		return false;
	}
}

bool fire_bullet(vec3_t vecSrc, vec3_t shootAngles, float flDistance, float flPenetration, int nPenetrationCount, char* iBulletType,
	float& fCurrentDamage, float flRangeModifier, c_base_entity* pevAttacker/*, float xSpread, float ySpread*/)
{
	//float fCurrentDamage = iDamage;   // damage of the bullet at it's current trajectory
	float flCurrentDistance = 0.0;  //distance that the bullet has traveled so far

	vec3_t vecDirShooting, vecRight, vecUp;
	global_math::angle_vectors(shootAngles, &vecDirShooting, &vecRight, &vecUp);

	// MIKETODO: put all the ammo parameters into a script file and allow for CS-specific params.
	float flPenetrationPower = 0;		// thickness of a wall that this bullet can penetrate
	float flPenetrationDistance = 0;	// distance at which the bullet is capable of penetrating a wall
	float flDamageModifier = 0.5f;		// default modification of bullets power after they go through a wall.
	float flPenetrationModifier = 1.0f;

	auto sv_penetration_type = I::game_cvar->find_var("sv_penetration_type");

	GetBulletTypeParameters(flPenetrationPower, flPenetrationDistance, iBulletType, sv_penetration_type->get_bool());

	if(sv_penetration_type->get_int() == 1)
		flPenetrationPower = flPenetration;

	vec3_t vecDir = vecDirShooting;

	auto weapon = G::local_player->active_weapon();
	if (!weapon) return false;
	auto weaponData = weapon->get_cs_weapon_data();
	if (!weaponData) return false;

	VectorNormalize(vecDir);

	bool bFirstHit = true;
	c_base_entity* lastPlayerHit = nullptr;
	vec3_t vecWallBangHitStart, vecWallBangHitEnd;
	vecWallBangHitStart.Init();
	vecWallBangHitEnd.Init();
	bool bWallBangStarted = false;
	bool bWallBangEnded = false;
	bool bWallBangHeavyVersion = false;

	bool bBulletHitPlayer = false;

	bool bShotHitTeammate = false;

	float flDist_aim = 0;
	vec3_t vHitLocation = vec3_t(0, 0, 0);

	fCurrentDamage = weaponData->iDamage;

	while (fCurrentDamage >= 1.f && nPenetrationCount > 0)
	{
		vec3_t vecEnd = vecSrc + vecDir * (flDistance - flCurrentDistance);
		c_trace tr;

		UTIL_TraceLineIgnoreTwoEntities(vecSrc, vecEnd, cs_mask_shoot | contents_hitbox, G::local_player, lastPlayerHit, COLLISION_GROUP_NONE, &tr);
		{
			c_trace_filter_skip_two_entities filter(G::local_player, lastPlayerHit);

			// Check for player hitboxes extending outside their collision bounds
			const float rayExtension = 40.0f;
			UTIL_ClipTraceToPlayers(vecSrc, vecEnd + vecDir * rayExtension, cs_mask_shoot | contents_hitbox, &filter, &tr);
		}

		if (!flDist_aim)
		{
			flDist_aim = (tr.flFraction != 1.0) ? (tr.start - tr.end).Length() : 0;
		}

		if (flDist_aim)
		{
			vHitLocation = tr.end;
		}

		lastPlayerHit = tr.entity;

		if (lastPlayerHit)
		{
			if (lastPlayerHit->team() == G::local_player->team())
			{
				bShotHitTeammate = true;
			}

			bBulletHitPlayer = true;
		}

		if (tr.flFraction == 1.0f)
			break; // we didn't hit anything, stop tracing shoot

		surfacedata_t* pSurfaceData = I::phys_surface->surface_data(tr.surface.surface_props);
		int iEnterMaterial = pSurfaceData->game.material;

		if (flCurrentDistance > 3000.0f && weaponData->flPenetration > 0.f || pSurfaceData->game.fl_penetration_modifier < 0.1f)
			break;

		bool canDoDamage = (tr.hitGroup != hitgroup_gear && tr.hitGroup != hitgroup_generic);
		bool isPlayer = ((tr.entity)->GetClientClass()->class_id == 40);
		bool isEnemy = (tr.entity)->team() != G::local_player->team();
		
		flPenetrationModifier = pSurfaceData->game.fl_penetration_modifier;
		flDamageModifier = pSurfaceData->game.fl_damage_modifier;

		bool hitGrate = (tr.contents & contents_grate) != 0;

		flCurrentDistance += tr.flFraction * (flDistance - flCurrentDistance);
		fCurrentDamage *= pow(flRangeModifier, (flCurrentDistance / 500));

		if ((flCurrentDistance > flPenetrationDistance && flPenetration > 0) ||
			flPenetrationModifier < 0.1)
			nPenetrationCount = 0;

		int iDamageType = DMG_BULLET | DMG_NEVERGIB;
		c_base_combat_weapon* pActiveWeapon = G::local_player->active_weapon();
		if (pActiveWeapon && pActiveWeapon->get_item_definition_index() == item_definition_index::WEAPON_TASER)
			iDamageType = DMG_SHOCK | DMG_NEVERGIB;

		if (canDoDamage && isPlayer && isEnemy)
		{
			scale_damage(tr.entity, tr, weaponData, fCurrentDamage);
			return true;
		}

		if (isPlayer && tr.entity == G::local_player)
		{
			tr.hitGroup = 1;
			return true;
		}

		bool bulletStopped = HandleBulletPenetration(flPenetration, iEnterMaterial, hitGrate, tr, vecDir, pSurfaceData, flPenetrationModifier,
			flDamageModifier, false, iDamageType, flPenetrationPower, nPenetrationCount, vecSrc, flDistance,
			flCurrentDistance, fCurrentDamage);

		// [dkorus] bulletStopped is true if the bullet can no longer continue penetrating materials
		if (bulletStopped)
			break;
	}

	return false;
}

bool autowall::can_hit(c_base_entity* ent, vec3_t& point, float& damage, vec3_t pos, c_config::rage::_weapons cfg)
{
	vec3_t angles;
	vec3_t direction;
	vec3_t tmp = point - pos;

	global_math::vector_angles(tmp, angles);
	global_math::angle_vectors(angles, &direction);
	direction.NormalizeInPlace();

	auto weapon = G::local_player->active_weapon();
	if (!weapon) return false;
	auto pWeaponInfo = weapon->get_cs_weapon_data();
	if (!pWeaponInfo) return false;
	
	bool ret = fire_bullet(pos, angles, pWeaponInfo->flRange, pWeaponInfo->flPenetration, 4, pWeaponInfo->szBulletType, damage, pWeaponInfo->flRangeModifier, G::local_player/*, x0 + x1[0], y0 + y1[0]*/);

	if (damage < cfg.mindmg && damage < ent->health() + 4.f)
		return false;

	return ret;
}