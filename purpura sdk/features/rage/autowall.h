#pragma once

#include "../../sdk/sdk.h"
#include "../../interfaces/interfaces.h"
#include "../../utilities/globals.h"
#include "../../utilities/math.h"
#include "../../menu/settings.h"

namespace autowall {
	bool can_hit(c_base_entity*, vec3_t&, float&, vec3_t, c_config::rage::_weapons);
}