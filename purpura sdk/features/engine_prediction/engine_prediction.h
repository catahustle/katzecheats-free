#pragma once

#include "../../interfaces/interfaces.h"

#include "../../sdk/entities.h"

namespace features {
	class engine_prediction {
	public:
		vec3_t unpred_eyepos;

		void start_prediction(c_base_entity* local_player, c_usercmd* cmd);
		void end_prediction(c_base_entity* local_player);
	};

	extern engine_prediction* engine_pred;
}