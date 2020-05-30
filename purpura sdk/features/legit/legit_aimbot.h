#pragma once

#include "../../interfaces/interfaces.h"

namespace features {
	class legit_aimbot {
	public:
		void on_create_move();
		void autopistol();

		float stop_time = 0.0f;
	};

	extern legit_aimbot* legit;
}