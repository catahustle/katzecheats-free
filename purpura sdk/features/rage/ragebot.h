#pragma once

#include "../../utilities/vector.h"
#include "../../sdk/entities.h"

namespace features {
	class ragebot {
	public:
		void on_create_move();
		void apply_aa();
		bool hitchance(vec3_t, float, c_base_entity*);
	};

	extern ragebot* rage;
}