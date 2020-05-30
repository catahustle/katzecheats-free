#pragma once

#include "../sdk/c_input.h"
#include "../sdk/entities.h"

namespace global_variables {
	extern c_usercmd*		global_cmd;
	extern c_base_entity*	local_player;
	extern bool				b_send_packet;
	extern vec3_t			original_angles;
	extern int				ShotsFired[65];
	extern int				ShotsHit[65];
}

#define G global_variables
#define I interfaces