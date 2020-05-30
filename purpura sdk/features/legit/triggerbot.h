#pragma once

#include "../../interfaces/interfaces.h"

namespace features {
	class legit_triggerbot {
	public:
		void on_create_move();
	};

	extern legit_triggerbot* triggerbot;
}