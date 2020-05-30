#pragma once

#include "../../interfaces/interfaces.h"

namespace features
{
	class legit_antiaim
	{
	public:
		void on_create_move();
		void resolve();
	};

	extern legit_antiaim* legit_aa;
}